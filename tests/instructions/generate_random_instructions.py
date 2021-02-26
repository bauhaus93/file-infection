#!/bin/env python3

import os
import random
import logging
import argparse
import time

from utility import try_decompile_instruction, setup_logger

logger = logging.getLogger()


def create_random_instruction_bytes():
    PREFIXES = [0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67]
    PREFIX_2BYTE = 0x0F
    PREFIX_3BYTE = [0x38, 0x3A]
    num_prefixes = random.choices(
        range(5), weights=list(map(lambda e: e ** 2, reversed(range(5))))
    )[0]
    prefixes = random.choices(PREFIXES, k=num_prefixes)
    instr_len = random.choices([1, 2, 3], weights=[3, 2, 1])[0]
    modrm = random.choice([True, False])
    sib = random.choices([True, False], weights=[1, 3])
    imm = random.choice([0, 1, 2, 4])
    binary = []
    binary.extend(prefixes)
    if instr_len == 1:
        binary.append(random.randint(0x00, 0xFF))
    elif instr_len == 2:
        binary.extend([PREFIX_2BYTE, random.randint(0x00, 0xFF)])
    elif instr_len == 3:
        binary.extend(
            [PREFIX_2BYTE, random.choice(PREFIX_3BYTE), random.randint(0x00, 0xFF)]
        )
    if modrm:
        binary.append(random.randint(0x00, 0xFF))
        if sib:
            binary.append(random.randint(0x00, 0xFF))
    for i in range(imm):
        binary.append(0xFF)
    return binary


def create_random_instruction():
    while True:
        instr = create_random_instruction_bytes()
        instr_str = try_decompile_instruction(instr, 32)
        if not instr_str is None:
            return instr_str


if __name__ == "__main__":
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Collects instructions from .s files in the given directory, then create random instructions out of it."
    )
    parser.add_argument(
        "--output-file",
        metavar="File",
        type=str,
        required=True,
        help="Name of the generated file",
    )

    parser.add_argument(
        "--seed",
        type=str,
        required=True,
        help="Seed used in rng",
    )

    parser.add_argument(
        "--count",
        type=int,
        required=True,
        help="Number of generated instructions",
    )
    args = parser.parse_args()

    random.seed(args.seed)

    output_file = os.path.abspath(args.output_file)

    logger.info(f"Creating {args.count} random instructions from seed '{args.seed}'")

    instructions = set()
    last_print = time.time()
    while len(instructions) < args.count:
        instructions.add(create_random_instruction())
        if time.time() - last_print > 2:
            logger.info(f"Progress: {100 * len(instructions) // args.count}%")
            last_print = time.time()
    with open(output_file, "w") as f:
        f.write("\n".join(sorted(instructions)))
