#!/bin/env python3

import argparse
import logging
import os
import sys

from call import verify_call_targets
from disassembly import parse_file
from instruction import Instruction
from similarity import check_function_sanity, verify_instruction_similarity

_FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
_DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"


def setup_logger():
    logging.basicConfig(level=logging.INFO, format=_FORMAT, datefmt=_DATE_FORMAT)


_logger = logging.getLogger(__name__)


def create_argument_parser():
    parser = argparse.ArgumentParser(
        description="Compares two binary files by checking if they have"
        "relevant differences in their disassembly."
    )

    unmod_group = parser.add_mutually_exclusive_group(required=True)
    unmod_group.add_argument(
        "--unmodified-disassembly",
        metavar="File",
        type=str,
        help="Name of the unmodified file, containing disassembly of the code",
    )
    unmod_group.add_argument(
        "--unmodified-binary",
        metavar="File",
        type=str,
        help="Name of the unmodified file, containing a binary dump of the code",
    )

    mod_group = parser.add_mutually_exclusive_group(required=True)
    mod_group.add_argument(
        "--modified-disassembly",
        metavar="File",
        type=str,
        help="Name of the modified file, containing disassembly of the code",
    )
    mod_group.add_argument(
        "--modified-binary",
        metavar="File",
        type=str,
        help="Name of the modified file, containing a binary dump of the code",
    )

    parser.add_argument(
        "--check-type",
        type=str,
        choices=["instruction_count", "function_sanity", "call_targets"],
        required=True,
        help="Determines, which kind of check should be run",
    )
    return parser


def parse_files(args: argparse.ArgumentParser) -> (Instruction, Instruction):
    if args.unmodified_disassembly:
        unmod_disassembly_path = os.path.abspath(args.unmodified_disassembly)
        unmod_instructions = parse_file(unmod_disassembly_path, "disasm")
    elif args.unmodified_binary:
        unmod_binary_path = os.path.abspath(args.unmodified_binary)
        unmod_instructions = parse_file(unmod_binary_path, "binary")

    if args.modified_disassembly:
        mod_disassembly_path = os.path.abspath(args.modified_disassembly)
        mod_instructions = parse_file(mod_disassembly_path, "disasm")
    elif args.modified_binary:
        mod_binary_path = os.path.abspath(args.modified_binary)
        mod_instructions = parse_file(mod_binary_path, "binary")
    return unmod_instructions, mod_instructions


def main():
    setup_logger()
    parser = create_argument_parser()
    args = parser.parse_args()

    unmod_instructions, mod_instructions = parse_files(args)

    if args.check_type == "instruction_count":
        differences = verify_instruction_similarity(
            unmod_instructions, mod_instructions
        )
        if differences:
            _logger.error("Instruction difference not zero: %d", differences)
            return False
    elif args.check_type == "call_targets":
        differences = verify_call_targets(unmod_instructions, mod_instructions)
        if differences > 0:
            _logger.error("Call targets difference not zero: %d", differences)
            return False
    elif args.check_type == "function_sanity":
        try:
            destroyed_functions = check_function_sanity(
                unmod_instructions, mod_instructions
            )
        except Exception as e:
            _logger.error("Could not check function sanity: %s", e)
            return False

        if len(destroyed_functions) > 0:
            _logger.error(
                "Got some destroyed functions: %s", ", ".join(destroyed_functions)
            )
            return False
    else:
        _logger.error("No check type was given!")
        return False
    return True


if __name__ == "__main__":
    SUCCESS = main()
    if not SUCCESS:
        sys.exit(1)
