#!/bin/env python3

import argparse
import logging
import os

from disassembly import disassemble_file
from line import create_linewise_comparison

_FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
_DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"


def setup_logger():
    logging.basicConfig(level=logging.INFO, format=_FORMAT, datefmt=_DATE_FORMAT)


_logger = logging.getLogger(__name__)


def main():
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Creates a line by line comparison of two disassemblies"
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
    parser.add_argument(
        "--output",
        metavar="File",
        type=str,
        required=True,
        help="Output file of the comparison",
    )
    args = parser.parse_args()

    unmodified_file = os.path.abspath(args.unmodified_file)
    modified_file = os.path.abspath(args.modified_file)
    output_file = os.path.abspath(args.output)

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

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(create_linewise_comparison(unmod_disassembly, mod_disassembly))
    _logger.info("Created linewise comparison in '%s'", output_file)


if __name__ == "__main__":
    main()
