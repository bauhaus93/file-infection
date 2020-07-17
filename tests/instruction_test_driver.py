#!/bin/env python3

import argparse
import os
import logging
import subprocess
import tempfile
import uuid
import ctypes

from utility import setup_logger, check_instructions

logger = logging.getLogger()

if __name__ == "__main__":
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Compiles each line from the asm file, then checks the compiled size against the size determined by the disassembly library."
    )
    parser.add_argument(
        "source_file",
        metavar="FILE",
        type=str,
        help="File for which to check instruction sizes",
    )
    parser.add_argument(
        "--bit-width",
        choices=[32, 64],
        type=int,
        default=32,
        help="Set the bit width used for compilation of the instructions.",
    )
    parser.add_argument(
        "--disasm-lib",
        metavar="FILE",
        type=str,
        required=True,
        help="Disassembler libary to be checked",
    )
    args = parser.parse_args()

    source_path = os.path.abspath(args.source_file)
    if not os.path.isfile(source_path):
        logger.error(f"Could not find source file '{source_path}'")
        exit(1)

    lib_path = os.path.abspath(args.disasm_lib)
    if not os.path.isfile(lib_path):
        logger.error(f"Could not find disassembler libary '{lib_path}')")
        exit(1)
    if check_instructions(source_path, args.bit_width, lib_path):
        exit(0)
    else:
        exit(1)
