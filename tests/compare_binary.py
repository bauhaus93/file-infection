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

    diss_list = sorted(
        map(lambda kv: (kv[0], kv[1]), diss_dict.items()), key=lambda kv: kv[1]
    )
    diss_list = filter(
        lambda e: not (any(map(lambda w: w in e[0], FILTER_WORDS))), diss_list)

    return list(diss_list)

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

    unmod_list = filter_disassembly(unmod_disassembly)
    mod_list = filter_disassembly(mod_disassembly)
    if len(unmod_list) != len(mod_list):
        logger.error(f"Number of relevant instructions changed: Unmodified: {len(unmod_list)}, modified: {len(mod_list)}")
        exit(1)
    for (k_u, v_u), (k_m, v_m) in zip(unmod_list, mod_list):
        if k_u != k_m:
            logger.error("Sorted disassembly order changed")
            exit(1)
        elif v_u != v_m:
            logger.error(f"Number of operation occurences changed for '{k_u}': Unmodified is {v_u}, modified is {v_m}")
            exit(1)

