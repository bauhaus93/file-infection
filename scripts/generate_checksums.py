#!/usr/bin/env python3
import logging
import argparse
import os

import util

def create_checksum(string):
    a = 1
    b = 0

    for c in string:
        a = (a + ord(c)) % 65521
        b = (b + a) % 65521#

    return ((b << 16) + a) & 0xFFFFFFFF

def create_checksum_map(input_filename):
    checksum_map = {}
    with open(input_filename, "r") as f:
        for line in f:
            name = line.rstrip()
            cs = create_checksum(name)
            if cs in checksum_map :
                if name != checksum_map[cs]:
                    logger.warn("Found hash collision: {} with {} -> 0x{:08X}".format(name, checksum_map[cs], cs))
                else:
                    logger.warn("Found duplicate string: {}".format(name))
            else:
                checksum_map[cs] = name
    return checksum_map

def create_header_file(header_filename, checksum_map):
    safeguard = util.create_safeguard(header_filename)
    with open(header_filename, "w") as f:
        f.write(f"#ifndef {safeguard}\n#define {safeguard}\n")
        for cs, name in sorted(checksum_map.items(), key = lambda e: e[1].lower()):
            f.write("#define CS_{} (0x{:X})\n".format(name.upper().replace(".", "_"), cs))
            logger.debug("Checksum for {}: 0x{:X}".format(name, cs))
        f.write(f"#endif // {safeguard}")

if __name__ == "__main__":
    util.setup_logger()
    logger = logging.getLogger()

    parser = argparse.ArgumentParser("Generate checksums from strings in a file, creating a c header files with #defines of the checksums.")
    parser.add_argument("--input-file",
                        metavar = "FILE",
                        help = "Specify the file input file, containing one string per line.",
                        required = True)
    parser.add_argument("--output-file",
                        metavar = "FILE",
                        help = "Specify the output file, being a c header file.",
                        required = True)
    args = parser.parse_args()

    checksum_map = create_checksum_map(args.input_file)
    create_header_file(args.output_file, checksum_map)
    logger.info("Created '{}', containing {} checksums".format(args.output_file, len(checksum_map)))
