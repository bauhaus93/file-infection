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

PREFIXES = ["rep", "repe", "repne", "repnz", "repz", "lock"]

TARGET_TEMPLATES = [
    *STANDARD_REGS,
    *SSE_MMX_REGS,
    *DEBUG_REGS,
    *CONTROL_REGS,
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
        for (instr, valid) in executor.map(lambda instr: (instr, compiles_any(instr, arguments)), instructions):
            if valid:
                valids.append(instr)
            i += 1
            if time.time() - last_print  > 5:
                last_print = time.time()
                diff = time.time() - start
                if diff > 0:
                    rate = i / diff
                    if rate > 0:
                        eta = (len(instructions) - i) / rate
                        eta = f"{int(eta // 60):2}m {int(eta % 60):2}s" if eta >= 60 else f"{int(eta):2}s"
                    else:
                        eta = "NA"
                else:
                    eta = "NA"
                logger.info(f"Progress: {100. * i / len(instructions):4.1f}% | ETA: {eta:10s}")
            
    return valids


def filter_argument_count(instructions, arg_count):
    combinations = list(itertools.combinations(TARGET_TEMPLATES, arg_count))
    return filter_instructions(instructions, combinations)


def filter_immediate(instructions, arg_count):
    IMMEDIATE_ARGS = ["0FFh", "0FFFFh", "0FFFFFFFFh"]
    if arg_count == 1:
        combinations = IMMEDIATE_ARGS
    elif arg_count == 2:
        combinations = list(itertools.product(TARGET_TEMPLATES, IMMEDIATE_ARGS))
    else:
        logger.error(f"Only 1 or 2 args allowed")
        return []
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


def random_register():
    return random.choice([*STANDARD_REGS, *SSE_MMX_REGS])


def random_prefix():
    return random.choice(PREFIXES)


def create_direct_memory():
    return random_hex_string()


def create_reg_offset_memory():
    op = random.choice(["+", "-"])
    return f"{random_address_register()} {op} {random_hex_string()}"


def create_sib_memory():
    op = random.choice(["+", "-"])
    size = random.choice(["1", "2", "4"])
    return f"{random_address_register()} {op} {size} * {random_address_register()}"


def create_random_value():
    if random.random() < 0.5:
        return random_hex_string()
    else:
        return random_register()


def create_random_mem_location():
    n = random.random()
    if n < 0.1:
        loc = create_direct_memory()
    elif n < 0.5:
        loc = create_reg_offset_memory()
    else:
        loc = create_sib_memory()
    return f"[{loc}]"


def create_random_instruction(instructions):
    num_args = random.choice([0, *([1] * 10), *([2] * 10)])
    if num_args > 0:
        dest_mem = random.choice([True, False])
        if num_args == 2:
            if dest_mem:
                src_mem = False
            else:
                src_mem = random.choice([True, False])

            if src_mem:
                src = create_random_mem_location()
            else:
                src = create_random_value()

        if dest_mem:
            dest = create_random_mem_location()
        else:
            dest = create_random_value()

    if random.random() < 0.05:
        line = random_prefix() + " "
    else:
        line = ""

    line += random.choice(instructions)

    if num_args >= 1:
        line += f" {dest}"
    if num_args >= 2:
        line += f", {src}"
    return line


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


def create_random_instructions_file(filename, num, instructions):
    with open(filename, "w") as f:
        logger.info(f"Creating {num} random instructions")
        for i in range(num):
            instr = create_random_instruction(instructions)
            while compile_instruction(instr, 32, silent_error=True) is None:
                instr = create_random_instruction(instructions)
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

    imm1_path = os.path.join(root_dir, "imm1.txt")
    imm1 = get_filtered_instructions(imm1_path, lambda: filter_immediate(arg1, 1))

    imm2_path = os.path.join(root_dir, "imm2.txt")
    imm2 = get_filtered_instructions(imm2_path, lambda: filter_immediate(arg2, 2))


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
    output_file = os.path.abspath(args.output_file)
    instructions = discover_instructions(input_dir)

    logger.info(f"Seed is '{args.seed}'")
    logger.info(f"Instruction pool: {len(instructions)}")

    collect_instruction_info(instructions, os.path.abspath("info"))

    # create_random_instructions_file(output_file, args.count, instructions)
