#include "discovery.h"
#include "../disasm/disasm.h"
#include "../disasm/instruction.h"
#include "../utility.h"
#include "block.h"
#include "prepare.h"
#include "reference.h"

static BlockList *check_block(void *start_address, BlockList *block_list);

static void *get_next_entrypoint(ReferenceList **pending_calls,
                                 ReferenceList **checked_calls);
static ReferenceList *find_calls_in_block(const Block *block);
static ReferenceList *collect_calls_from_blocks(BlockList *block_list);
static ReferenceList *collect_new_calls(ReferenceList *pending_calls,
                                        ReferenceList *checked_calls,
                                        BlockList *block_list);
BlockList *discover_blocks(void **entrypoints) {
  BlockList *blocks = NULL;

  ReferenceList *pending_calls = NULL;
  for (int i = 0; entrypoints[i] != NULL; i++) {
    pending_calls = push_reference(entrypoints[i], NULL, pending_calls);
  }
  ReferenceList *checked_calls = NULL;

  while (pending_calls != NULL) {
    void *entrypoint = get_next_entrypoint(&pending_calls, &checked_calls);

    blocks = check_block(entrypoint, blocks);
    if (blocks == NULL) {
      break;
    }
    if (pending_calls == NULL) {
      pending_calls = collect_new_calls(pending_calls, checked_calls, blocks);
    }
  }
  free_reference_list(pending_calls);
  free_reference_list(checked_calls);

  blocks = prepare_blocks(blocks);

  return blocks;
}

static BlockList *check_block(void *start_address, BlockList *block_list) {
  Disassembler disasm;
  setup_disasm(start_address, &disasm);

  block_list = push_block(start_address, block_list);
  Block *block = top_block(block_list);

  bool found_end = false;
  while (next_instruction(&disasm)) {
    const Instruction *instr = get_current_instruction(&disasm);
    block->end = instr->end;
    block->last_instruction = instr->start;
    if (is_return(instr) || is_endbr(instr)) {
      block->dest = NULL;
      block->dest_alternative = NULL;
      found_end = true;
      break;
    } else if (is_jump(instr)) {
      void *target = get_jump_target(instr);
      if (is_conditional_jump(instr)) {
        block->dest = block->end;
        block->dest_alternative = target;
        block_list = check_block(instr->end, block_list);
      } else if (is_unconditional_jump(instr)) {
        block->dest = target;
        block->dest_alternative = NULL;
      }
      if (target != NULL) {
        Block *target_block = find_block_for_address(target, block_list);
        if (target_block == NULL) {
          block_list = check_block(target, block_list);
        } else if (target_block->start != target) { // split up existing block
          block_list = push_block(target, block_list);
          Block *new_block = top_block(block_list);
          new_block->end = target_block->end;
          new_block->dest = target_block->dest;
          new_block->dest_alternative = target_block->dest_alternative;
          new_block->last_instruction = target_block->last_instruction;

          target_block->end = target;
          target_block->dest = target;
          target_block->last_instruction = NULL;
        }
      }
      found_end = true;
      break;
    }
  }
  if (!found_end) {
    PRINT_DEBUG("Block ended with invalid instruction! @ %p", block->end);
    free_block_list(block_list);
    return NULL;
  }

  return block_list;
}

static void *get_next_entrypoint(ReferenceList **pending_calls,
                                 ReferenceList **checked_calls) {
  if (*pending_calls != NULL) {
    void *entrypoint = top_reference_dest(*pending_calls);
    *pending_calls = pop_reference(*pending_calls);
    *checked_calls = push_reference(entrypoint, NULL, *checked_calls);
    return entrypoint;
  }
  return NULL;
}

static ReferenceList *find_calls_in_block(const Block *block) {
  Disassembler disasm;
  setup_disasm(block->start, &disasm);

  ReferenceList *references = NULL;

  while (next_instruction(&disasm)) {
    const Instruction *instr = get_current_instruction(&disasm);
    if (instr->end > block->end) {
      break;
    }
    if (is_call(instr)) {
      void *target = get_call_target(instr);
      if (target != NULL) {
        references = push_reference(target, NULL, references);
      }
    }
  }
  return references;
}

static ReferenceList *collect_new_calls(ReferenceList *pending_calls,
                                        ReferenceList *checked_calls,
                                        BlockList *block_list) {
  ReferenceList *new_calls = collect_calls_from_blocks(block_list);
  while (new_calls != NULL) {
    void *new_target = top_reference_dest(new_calls);
    if (!reference_in_list(new_target, pending_calls) &&
        !reference_in_list(new_target, checked_calls)) {
      pending_calls = push_reference(new_target, NULL, pending_calls);
    }
    new_calls = pop_reference(new_calls);
  }
  return pending_calls;
}

static ReferenceList *collect_calls_from_blocks(BlockList *block_list) {
  ReferenceList *calls = NULL;
  for (BlockList *ptr = block_list; ptr != NULL; ptr = ptr->next) {
    ReferenceList *block_calls = find_calls_in_block(ptr->block);
    for (ReferenceList *call = block_calls; call != NULL; call = call->next) {
      if (!reference_in_list(call->dest, calls)) {
        calls = push_reference(call->dest, NULL, calls);
      }
    }
  }
  return calls;
}
