import logging

from instruction import (Instruction, count_different_instructions,
                         get_labeled_bytecode, get_labels)

_logger = logging.getLogger(__name__)


def verify_instruction_similarity(
    instructions_original: list[Instruction], instructions_modified: list[Instruction]
) -> int:
    unmod_dict = count_different_instructions(instructions_original)
    mod_dict = count_different_instructions(instructions_modified)

    new_appeared = 0
    new_missing = 0
    instructions = set([*unmod_dict.keys(), *mod_dict.keys()])

    # jmps  get discarded when they would jump to their subsequent block (-> 2 blocks get merged)
    # so ignore them for this test
    if "jmp" in instructions:
        instructions.remove("jmp")
    differences = []
    for key in instructions:

        unmod_count = unmod_dict.get(key, 0)
        mod_count = mod_dict.get(key, 0)
        if unmod_count != mod_count:
            differences.append((key, unmod_count, mod_count))
            if mod_count > unmod_count:
                new_appeared += mod_count - unmod_count
            elif mod_count < unmod_count:
                new_missing += unmod_count - mod_count

    for key, unmod_count, mod_count in sorted(differences, key=lambda e: e[2] - e[1]):
        _logger.error(
            "Amount of instruction occurences changed "
            "| %10s | original: %4d | copied: %4d | delta: %4d",
            key,
            unmod_count,
            mod_count,
            mod_count - unmod_count,
        )

    _logger.error("Instruction delta: +%d/-%d", new_appeared, new_missing)
    return new_appeared + new_missing


def check_function_sanity(
    instructions_original: list[Instruction], instructions_modified: list[Instruction]
) -> int:
    labels = get_labels(instructions_original)
    if not get_labels(instructions_original):
        raise Exception("Original dissassembly contains no labels to check against!")

    labeled_bytecode = get_labeled_bytecode(instructions_original)
    for label, bytecode in labeled_bytecode.items():
        _logger.info("label = %s, bytecode = %s", label, bytecode)

    raise NotImplementedError("TODO: Find matching functions in modified code")
