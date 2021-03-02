#!/bin/env python3

from functools import reduce
import os
import re
import logging
import argparse
import tempfile
import subprocess


def setup_logger():
    FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
    DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"
    logging.basicConfig(level=logging.INFO, format=FORMAT, datefmt=DATE_FORMAT)


logger = logging.getLogger()


def filter_disassembly(disassembly):
    FILTER_WORDS = [
        "call",
        "jg",
        "jnz",
        "jmp",
        "jnc",
        "jnl",
        "jl",
        "jns",
        "js",
        "jng",
        "nop",
        "jz",
        "jna",
        "ja",
        "jae",
        "jb",
        "jbe",
        "jc",
        "jnae",
    ]
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

    return dict(
        filter(
            lambda kv: not (any(map(lambda w: w in kv[0], FILTER_WORDS))),
            diss_dict.items(),
        )
    )


def disassemble_file(filename):
    result = subprocess.run(["ndisasm", "-b32", filename], capture_output=True)
    if result.returncode != 0:
        logger.error(
            f"Could not disassemble file '{filename}': {result.stderr.decode('utf-8')}"
        )
        return None
    return result.stdout.decode("utf-8")


def collect_call_targets(disassembly):
    pat = re.compile("call (0x[0-9a-fA-F]+)")
    targets = {}
    for m in pat.finditer(disassembly):
        targets[int(m.group(1), 16)] = targets.get(int(m.group(1), 16), 0) + 1
    return targets


def check_call_targets(original_disassembly, modified_disassembly):
    orig_targets = collect_call_targets(original_disassembly)
    mod_targets = collect_call_targets(modified_disassembly)

    targets = set([*orig_targets.keys(), *mod_targets.keys()])
    diff_count = 0
    total = 0
    for t in targets:
        c_orig = orig_targets.get(t, 0)
        c_mod = mod_targets.get(t, 0)
        if c_orig == 0:
            logger.error(f"New call\t\t| amount: {c_mod:3d} | target:  0x{t:08X}")
        if c_mod == 0:
            logger.error(f"Missing call\t| amount: {c_orig:3d} | target:  0x{t:08X}")
        diff_count += abs(c_orig - c_mod)
        total += c_orig + c_mod
    return 1.0 - diff_count / total


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
        logger.error(f"Could not disassemble '{modified_file}'")
        exit(1)

    similarity = check_call_targets(unmod_disassembly, mod_disassembly)
    if similarity < 1.0:
        logger.error(f"Call targets similarity: {similarity * 100:.1f}%")
        exit(1)

    unmod_dict = filter_disassembly(unmod_disassembly)
    mod_dict = filter_disassembly(mod_disassembly)
    only_old = set(unmod_dict.keys()).difference(set(mod_dict.keys()))
    only_new = set(mod_dict.keys()).difference(set(unmod_dict.keys()))

    total = 0
    diff_count = 0
    new_appeared = 0
    for key in set([*unmod_dict.keys(), *mod_dict.keys()]):
        unmod_count = unmod_dict.get(key, 0)
        mod_count = mod_dict.get(key, 0)
        if unmod_count != mod_count:
            logger.error(
                f"Amount of instruction occurences changed for '{key}': original: {unmod_count}, copied: {mod_count}"
            )
            if mod_count > unmod_count:
                # print(key, mod_count, unmod_count)
                new_appeared += mod_count - unmod_count
            diff_count += abs(unmod_count - mod_count)
            total += unmod_count
    logger.error(f"Difference: {100. * diff_count / total:.2f}%")
    logger.error(f"Newly appread: {100. * new_appeared / total:.2f}%")
    if diff_count > 0:
        exit(1)
