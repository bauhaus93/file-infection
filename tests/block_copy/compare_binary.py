#!/bin/env python3

import argparse
import logging
import os
import sys

from call import verify_call_targets
from disassembly import disassemble_file
from similarity import verify_instruction_similarity

_FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
_DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"


def setup_logger():
    logging.basicConfig(level=logging.INFO, format=_FORMAT, datefmt=_DATE_FORMAT)


_logger = logging.getLogger(__name__)


def main():
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Compares two binary files by checking if they have"
        "relevant differences in their disassembly."
    )
    parser.add_argument(
        "--unmodified-file",
        metavar="File",
        type=str,
        required=True,
        help="Name of the unmodified file",
    )
    parser.add_argument(
        "--modified-file",
        metavar="File",
        type=str,
        required=True,
        help="Name of the modified file",
    )

    args = parser.parse_args()

    unmodified_file = os.path.abspath(args.unmodified_file)
    modified_file = os.path.abspath(args.modified_file)

    _logger.info("Original file: '%s'", unmodified_file)
    _logger.info("Modified file: '%s'", modified_file)

    unmod_disassembly = disassemble_file(unmodified_file)
    if unmod_disassembly is None:
        _logger.error("Could not disassemble '%s'", unmodified_file)
        return False

    mod_disassembly = disassemble_file(modified_file)
    if mod_disassembly is None:
        _logger.error("Could not disassemble '%s'", modified_file)
        return False

    success = True

    differences = verify_instruction_similarity(unmod_disassembly, mod_disassembly)
    if differences:
        _logger.error("Instruction difference not zero: %d", differences)
        success = False

    differences = verify_call_targets(unmod_disassembly, mod_disassembly)
    if differences > 0:
        _logger.error("Call targets difference not zero: %d", differences)
        success = False

    return success


if __name__ == "__main__":
    SUCCESS = main()
    if not SUCCESS:
        sys.exit(1)
