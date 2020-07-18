#!/bin/env python3

import os
import re
import logging
import argparse
import random
import tempfile
import uuid
import itertools
import time
from concurrent.futures import ThreadPoolExecutor

from utility import check_instructions, setup_logger, compile_instruction

logger = logging.getLogger()


def create_standard_regs():
    regs = ["edi", "esi", "eip", "esp", "ebp"]
    for l in ["a", "b", "c", "d"]:
        regs.append(f"e{l}x")
        regs.append(f"{l}x")
        regs.append(f"{l}h")
        regs.append(f"{l}l")
    return regs


def create_sse_mmx_regs():
    regs = []
    for i in range(0, 8):
        regs.append(f"xmm{i}")
        regs.append(f"mm{i}")
    return regs


PREFIX_REGEX = re.compile("(rep(e|ne|nz|z)?)|(lock)", re.IGNORECASE)
ADDRESS_REGS = ["eax", "ebx", "ecx", "esi", "edi", "esp", "ebp", "eip"]
STANDARD_REGS = create_standard_regs()
SSE_MMX_REGS = create_sse_mmx_regs()
DEBUG_REGS = list([f"dr{i}" for i in range(7)])
CONTROL_REGS = list([f"dr{i}" for i in range(7)])
SEGMENTS = ["fs", "gs", "cs", "ss", "ds", "es"]

PREFIXES = ["rep", "repe", "repne", "repnz", "repz", "lock"]

TARGET_TEMPLATES = [
    "eax",
    "ax",
    "al",
    "xmm0",
    "mm0",
    "dr0",
    "cr0",
    "0",
    "[0]",
]


def compiles_any(instruction, argument_combinations):
    return any(
        compile_instruction(f"{instruction} " + ", ".join(args), 32, silent_error=True)
        != None
        for args in argument_combinations
    )


def filter_instructions(instructions, arguments):
    logger.info(
        f"Filtering {len(instructions)} instructions against {len(arguments)} argument combinations"
    )
    valids = []
    start = time.time()

    i = 0
    last_print = time.time()
    with ThreadPoolExecutor() as executor:
        for (instr, valid) in executor.map(
            lambda instr: (instr, compiles_any(instr, arguments)), instructions
        ):
            if valid:
                valids.append(instr)
            i += 1
            if time.time() - last_print > 5:
                last_print = time.time()
                diff = time.time() - start
                if diff > 0:
                    rate = i / diff
                    if rate > 0:
                        eta = (len(instructions) - i) / rate
                        eta = (
                            f"{int(eta // 60):2}m {int(eta % 60):2}s"
                            if eta >= 60
                            else f"{int(eta):2}s"
                        )
                    else:
                        eta = "NA"
                else:
                    eta = "NA"
                logger.info(
                    f"Progress: {100. * i / len(instructions):4.1f}% | ETA: {eta:10s}"
                )
    return valids


def filter_argument_count(instructions, arg_count):
    if arg_count <= 2:
        combinations = list(itertools.combinations(TARGET_TEMPLATES, arg_count))
    elif arg_count == 3:
        combinations = list(
            itertools.product(TARGET_TEMPLATES, TARGET_TEMPLATES, ["0FFh",])
        )
    else:
        logger.error("Only 0-3 args allowed")
        return []
    return filter_instructions(instructions, combinations)


def filter_immediate(instructions, arg_count):
    IMMEDIATE_ARGS = ["0FFh", "0FFFFh", "0FFFFFFFFh"]
    if arg_count == 1:
        combinations = list(map(lambda e: (e,), IMMEDIATE_ARGS))
    elif arg_count == 2:
        combinations = list(itertools.product(TARGET_TEMPLATES, IMMEDIATE_ARGS))
    else:
        combinations = list(
            itertools.product(TARGET_TEMPLATES, TARGET_TEMPLATES, IMMEDIATE_ARGS)
        )
    return filter_instructions(instructions, combinations)


def filter_dest_reg_standard(instructions):
    combinations = list(itertools.product(STANDARD_REGS, TARGET_TEMPLATES))
    combinations.extend(list(map(lambda e: (e,), STANDARD_REGS)))
    combinations.extend(
        list(itertools.product(STANDARD_REGS, TARGET_TEMPLATES, ["0FFh",]))
    )
    return filter_instructions(instructions, combinations)


def filter_dest_reg_sse_mmx(instructions):
    combinations = list(itertools.product(SSE_MMX_REGS, TARGET_TEMPLATES))
    combinations.extend(list(map(lambda e: (e,), SSE_MMX_REGS)))
    combinations.extend(
        list(itertools.product(SSE_MMX_REGS, TARGET_TEMPLATES, ["0FFh",]))
    )
    return filter_instructions(instructions, combinations)


def filter_src_reg_standard(instructions):
    combinations = list(itertools.product(TARGET_TEMPLATES, STANDARD_REGS))
    combinations.extend(
        list(itertools.product(TARGET_TEMPLATES, STANDARD_REGS, ["0FFh",]))
    )
    return filter_instructions(instructions, combinations)


def filter_src_reg_sse_mmx(instructions):
    combinations = list(itertools.product(TARGET_TEMPLATES, SSE_MMX_REGS))
    combinations.extend(
        list(itertools.product(TARGET_TEMPLATES, SSE_MMX_REGS, ["0FFh",]))
    )
    return filter_instructions(instructions, combinations)


def filter_dest_mem(instructions):
    MEM_ARGS = ["[0FFFFFFFFh]", "[eax]"]
    combinations = list(itertools.product(MEM_ARGS, TARGET_TEMPLATES))
    combinations.extend(list(map(lambda e: (e,), MEM_ARGS)))
    combinations.extend(list(itertools.product(MEM_ARGS, TARGET_TEMPLATES, ["0FFh",])))
    return filter_instructions(instructions, combinations)


def filter_src_mem(instructions):
    MEM_ARGS = ["[0FFFFFFFFh]", "[eax]"]
    combinations = list(itertools.product(TARGET_TEMPLATES, MEM_ARGS))
    combinations.extend(list(itertools.product(TARGET_TEMPLATES, MEM_ARGS, ["0FFh",])))
    return filter_instructions(instructions, combinations)


def random_hex_string():
    byte_len = random.choice([1, 2, 4])
    if byte_len == 4:
        num = random.randint(0, 0xFFFFFFFF)
    elif byte_len == 2:
        num = random.randint(0, 0xFFFF)
    else:
        num = random.randint(0, 0xFF)
    return "0" + hex(num)[2:] + "h"


def random_address_register():
    return random.choice(ADDRESS_REGS)


def random_standard_register():
    return random.choice(STANDARD_REGS)


def random_sse_mmx_register():
    return random.choice(SSE_MMX_REGS)


def random_segment():
    return random.choice(SEGMENTS)


def random_prefix():
    return random.choice(PREFIXES)


def random_memory_location():
    n = random.randint(0, 4)
    if n == 0:
        return f"[{random_hex_string()}]"
    elif n == 1:
        return f"[{random_address_register()}]"
    elif n == 2:
        return f"[{random_address_register()} + {random_hex_string()}]"
    elif n == 3:
        scale = str(random.choice([1, 2, 4]))
        return f"[{random_address_register()} + {scale} * {random_address_register()}]"
    elif n == 4:
        offset = random.choice([random_address_register(), random_hex_string()])
        return f"[{random_segment()}:{offset}]"


def choose_non_empty_intersection(fixed, options, instruction_info):
    return random.choice(
        list(
            filter(
                lambda opt: len(
                    instruction_info[fixed].intersection(instruction_info[opt])
                )
                > 0,
                options,
            )
        )
    )


def create_random_instruction(instruction_info):
    num_args = random.choices([0, 1, 2, 3], weights=[1, 5, 10, 5])[0]

    all_dest = ["dest_reg_std", "dest_reg_sse_mmx", "dest_mem"]
    if num_args == 0:
        instr = random.choice(list(instruction_info["arg0"]))
        dest_type = None
        src_type = None
    elif num_args == 1:
        all_dest.append("imm1")
        instr_argc = "arg1"
        dest_type = random.choice(
            list(
                filter(
                    lambda opt: len(
                        instruction_info[instr_argc].intersection(instruction_info[opt])
                    )
                    > 0,
                    all_dest,
                )
            )
        )
        instr = random.choice(
            list(instruction_info[instr_argc].intersection(instruction_info[dest_type]))
        )
        src_type = None
    elif num_args == 2:
        all_src = ["imm2", "src_mem", "src_reg_std", "src_reg_sse_mmx"]
        dest_src_combs = list(itertools.product(all_dest, all_src))
        instr_argc = "arg2"
        dest_type, src_type = random.choice(
            list(
                filter(
                    lambda opt_comb: len(
                        instruction_info[instr_argc]
                        .intersection(instruction_info[opt_comb[0]])
                        .intersection(instruction_info[opt_comb[1]])
                    )
                    > 0,
                    dest_src_combs,
                )
            )
        )
        instr = random.choice(
            list(
                instruction_info[instr_argc]
                .intersection(instruction_info[dest_type])
                .intersection(instruction_info[src_type])
            )
        )
    elif num_args == 3:
        all_src = ["imm3", "src_mem", "src_reg_std", "src_reg_sse_mmx"]
        dest_src_combs = list(itertools.product(all_dest, all_src))
        instr_argc = "arg3"
        dest_type, src_type = random.choice(
            list(
                filter(
                    lambda opt_comb: len(
                        instruction_info[instr_argc]
                        .intersection(instruction_info[opt_comb[0]])
                        .intersection(instruction_info[opt_comb[1]])
                    )
                    > 0,
                    dest_src_combs,
                )
            )
        )
        instr = random.choice(
            list(
                instruction_info[instr_argc]
                .intersection(instruction_info[dest_type])
                .intersection(instruction_info[src_type])
            )
        )

    args = []
    if dest_type == "dest_reg_std":
        args.append(random_standard_register())
    elif dest_type == "dest_reg_sse_mmx":
        args.append(random_sse_mmx_register())
    elif dest_type == "dest_mem":
        args.append(random_memory_location())
    elif dest_type == "imm1":
        args.append(random_hex_string())

    if src_type == "imm2":
        args.append(random_hex_string())
    elif src_type == "src_mem":
        args.append(random_memory_location())
    elif src_type == "src_reg_std":
        args.append(random_standard_register())
    elif src_type == "src_reg_sse_mmx":
        args.append(random_sse_mmx_register())

    if num_args == 3:
        args.append(random_hex_string()[:3] + "h")

    return (instr + " " + ", ".join(args)).strip()


def collect_instructions(file_name):
    instrs = []
    with open(file_name) as f:
        for line in f:
            comment_start = line.find(";")
            if comment_start > -1:
                line = line[:comment_start]
            parts = line.split(" ")
            if PREFIX_REGEX.search(parts[0]):
                if len(parts) > 1:
                    cmd = parts[1]
                else:
                    cmd = parts[0]
            else:
                cmd = parts[0]
            cmd = cmd.strip()
            if len(cmd) > 0:
                instrs.append(cmd)
    return instrs


def discover_instructions(directory):
    instrs = []
    for (dir_path, dir_name, file_names) in os.walk(directory):
        for file_name in file_names:
            if file_name.endswith(".s"):
                file_path = os.path.join(dir_path, file_name)
                instrs.extend(collect_instructions(file_path))
    return list(set(instrs).difference(set(["ud0", "ud1", "ud2"])))


def create_random_instructions_file(filename, num, instruction_info):
    with open(filename, "w") as f:
        logger.info(f"Creating {num} random instructions")
        for i in range(num):
            instr = create_random_instruction(instruction_info)
            while compile_instruction(instr, 32, silent_error=True) is None:
                instr = create_random_instruction(instruction_info)
            f.write(instr + "\n")
            if num >= 10 and i % (num // 10) == 0:
                logger.info(f"Progress: {100. * (i / num):.1f}%")

    return filename


def write_raw_instructions(instructions, filename):
    with open(filename, "w") as f:
        for instr in sorted(instructions):
            f.write(f"{instr}\n")


def read_raw_instructions(filename):
    with open(filename, "r") as f:
        return list(filter(lambda e: len(e) > 0, set(f.read().split("\n"))))
    return []


def get_filtered_instructions(target_file, fallback_function):
    if not os.path.isfile(target_file):
        logger.info(f"Getting filtered instruction from fallback...")
        filtered = fallback_function()
        logger.info(f"Got {len(filtered)} functions")
        write_raw_instructions(filtered, target_file)
    else:
        logger.info(f"Getting filtered instructions from file: '{target_file}'")
        filtered = read_raw_instructions(target_file)
        logger.info(f"Got {len(filtered)} functions")
    return set(filtered)


def collect_instruction_info(instructions, root_dir):
    if not os.path.isdir(root_dir):
        try:
            os.makedirs(root_dir)
        except OSError as e:
            logger.error(f"Could not create working dir: {e}")
            return

    instructions = set(instructions)

    arg0_path = os.path.join(root_dir, "arg0.txt")
    arg0 = get_filtered_instructions(
        arg0_path, lambda: filter_argument_count(instructions, 0)
    )

    arg1_path = os.path.join(root_dir, "arg1.txt")
    arg1 = get_filtered_instructions(
        arg1_path, lambda: filter_argument_count(instructions.difference(arg0), 1)
    )

    arg2_path = os.path.join(root_dir, "arg2.txt")
    arg2 = get_filtered_instructions(
        arg2_path,
        lambda: filter_argument_count(
            instructions.difference(arg0).difference(arg1), 2
        ),
    )

    arg3_path = os.path.join(root_dir, "arg3.txt")
    arg3 = get_filtered_instructions(
        arg3_path,
        lambda: filter_argument_count(
            instructions.difference(arg0).difference(arg1).difference(arg2), 3
        ),
    )

    imm1_path = os.path.join(root_dir, "imm1.txt")
    imm1 = get_filtered_instructions(imm1_path, lambda: filter_immediate(arg1, 1))

    imm2_path = os.path.join(root_dir, "imm2.txt")
    imm2 = get_filtered_instructions(imm2_path, lambda: filter_immediate(arg2, 2))

    imm3_path = os.path.join(root_dir, "imm3.txt")
    imm3 = get_filtered_instructions(imm3_path, lambda: filter_immediate(arg3, 3))

    dest_reg_std_path = os.path.join(root_dir, "dest_reg_std.txt")
    dest_reg_std = get_filtered_instructions(
        dest_reg_std_path,
        lambda: filter_dest_reg_standard(arg1.union(arg2).union(arg3)),
    )

    dest_reg_sse_mmx_path = os.path.join(root_dir, "dest_reg_sse_mmx.txt")
    dest_reg_sse_mmx = get_filtered_instructions(
        dest_reg_sse_mmx_path,
        lambda: filter_dest_reg_sse_mmx(arg1.union(arg2).union(arg3)),
    )

    src_reg_std_path = os.path.join(root_dir, "src_reg_std.txt")
    src_reg_std = get_filtered_instructions(
        src_reg_std_path, lambda: filter_src_reg_standard(arg2.union(arg3))
    )

    src_reg_sse_mmx_path = os.path.join(root_dir, "src_reg_sse_mmx.txt")
    src_reg_sse_mmx = get_filtered_instructions(
        src_reg_sse_mmx_path, lambda: filter_src_reg_sse_mmx(arg2.union(arg3))
    )

    dest_mem_path = os.path.join(root_dir, "dest_mem.txt")
    dest_mem = get_filtered_instructions(
        dest_mem_path, lambda: filter_dest_mem(arg1.union(arg2).union(arg3))
    )

    src_mem_path = os.path.join(root_dir, "src_mem.txt")
    src_mem = get_filtered_instructions(
        src_mem_path, lambda: filter_src_mem(arg2.union(arg3))
    )

    return {
        "arg0": arg0,
        "arg1": arg1,
        "arg2": arg2,
        "arg3": arg3,
        "imm1": imm1,
        "imm2": imm2,
        "imm3": imm3,
        "dest_reg_std": dest_reg_std,
        "dest_reg_sse_mmx": dest_reg_sse_mmx,
        "dest_mem": dest_mem,
        "src_reg_std": src_reg_std,
        "src_reg_sse_mmx": src_reg_sse_mmx,
        "src_mem": src_mem,
    }


if __name__ == "__main__":
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Collects instructions from .s files in the given directory, then create random instructions out of it."
    )
    parser.add_argument(
        "--input-dir",
        metavar="DIRECTORY",
        type=str,
        required=True,
        help="Directory in which to search for .s files",
    )

    parser.add_argument(
        "--info-dir",
        metavar="DIRECTORY",
        type=str,
        required=True,
        help="Where to save/load instruction information",
    )
    parser.add_argument(
        "--output-file",
        metavar="File",
        type=str,
        required=True,
        help="Name of the generated file",
    )

    parser.add_argument(
        "--seed", type=str, required=True, help="Seed used in rng",
    )

    parser.add_argument(
        "--count", type=int, required=True, help="Number of generated instructions",
    )
    args = parser.parse_args()

    logger.info("Creating random test file")
    random.seed(args.seed)
    input_dir = os.path.abspath(args.input_dir)
    info_dir = os.path.abspath(args.info_dir)
    output_file = os.path.abspath(args.output_file)
    instructions = discover_instructions(input_dir)

    logger.info(f"Seed is '{args.seed}'")
    logger.info(f"Instruction pool: {len(instructions)}")

    instruction_info = collect_instruction_info(instructions, info_dir)
    create_random_instructions_file(output_file, args.count, instruction_info)
