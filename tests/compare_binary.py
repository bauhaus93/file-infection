#!/bin/env python3

from functools import reduce
import os
import logging
import argparse
import tempfile
import subprocess

from utility import setup_logger

logger = logging.getLogger()


def filter_disassembly(disassembly):
    FILTER_WORDS = ["call", "jg", "jnz", "jmp","jnc", "jnl", "nop", "jz", "jna", "ja", "jae", "jb", "jbe", "jc", "jnae"]
    disassembly = sorted(
        list(
            filter(
                lambda l: len(l) > 0,
                [
                    list(filter(lambda e: len(e) > 0, line.split(" ")))[2:]
                    for line in disassembly.split("\n")
                ],
            )
        )
    )
    diss_dict = {}
    for line in disassembly:
        line = " ".join(line)
        diss_dict[line] = diss_dict.get(line, 0) + 1

    return dict(filter(
        lambda kv: not (any(map(lambda w: w in kv[0], FILTER_WORDS))), diss_dict.items()))

def disassemble_file(filename):
    result = subprocess.run(
        ["ndisasm", "-b32", filename], capture_output=True
    )
    if result.returncode != 0:
        return None
        logger.error(
            f"Could not disassemble file '{unmodified_file}': {result_unmod.stderr.decode('utf-8')}"
        )
    return result.stdout.decode("utf-8")

if __name__ == "__main__":
    setup_logger()
    parser = argparse.ArgumentParser(
        description="Compares two binary files by checking if they have relevant differences in their disassembly."
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

    unmod_disassembly = disassemble_file(unmodified_file)
    if unmod_disassembly is None:
        logger.error(f"Could not disassemble '{unmodified_file}'")
        exit(1)

    mod_disassembly = disassemble_file(modified_file)
    if mod_disassembly is None:
        logger.error(f"Could not disassemble '{unmodified_file}'")
        exit(1)

    unmod_dict = filter_disassembly(unmod_disassembly)
    mod_dict = filter_disassembly(mod_disassembly)
    if len(unmod_dict) != len(mod_dict):
        logger.error(f"Number of relevant instructions changed: Unmodified: {len(unmod_dict)}, modified: {len(mod_dict)}")
        exit(1)
    only_old = set(unmod_dict.keys()).difference(set(mod_dict.keys()))
    only_new = set(mod_dict.keys()).difference(set(unmod_dict.keys()))

    for old in only_old:
        logger.error(f"Instruction missing in modified: {old}")
    for new in only_new:
        logger.error(f"New instruction appeared in modified: {new}")

    if len(only_old) > 0 or len(only_new) > 0:
        exit(1)

    error_found = False
    for key in unmod_dict.keys():
        if unmod_dict[key] != mod_dict[key]:
            logger.error(f"Amount of instruction occurences changed for '{key}': Unmodified: {unmod_dict[key]}, modified: {mod_dict[key]}")
            error_found = True
    if error_found:
        exit(1)

