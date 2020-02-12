#!/usr/bin/env python3

import argparse
import os
import string
import logging
import random

import util

def read_words(filename):
    words = {}
    with open(filename, "r") as f:
        lines = f.readlines()
        word_id = 0
        for line in lines:
            word_symbol, word_value = line[:-1].split(" ", maxsplit = 1)
            if word_symbol.upper() in words:
                print("Symbol clash: " + word_symbol.upper())
                return {}
            words[word_symbol.upper()] = (word_id, word_value)
            word_id += 1
    return words

def generate_alphabet_map(word_map):
    alphabet = {}
    for word_symbol in word_map:
        for (i, c) in enumerate(word_map[word_symbol][1]):
            num = word_map[word_symbol][0] | (i << 8)
            if c in alphabet:
                alphabet[c].append(num)
            else:
                alphabet[c] = [num]
    return alphabet

def generate_function_get_word_length(word_map):
    code = "size_t get_string_length(uint16_t id) {\n"
    code += "switch(id) {\n"
    for word_symbol in word_map:
        code += f"case STRING_{word_symbol}: return {len(word_map[word_symbol][1])};\n"
    code += "default: return 0;\n}}"
    return code

def generate_function_generate_word(occurence_map):
    code = "size_t get_string(uint16_t id, char * dest, size_t dest_size) {\n"
    code += "size_t len = get_string_length(id);\n"
    code += "if (len == 0 || len >= dest_size) { return 0; }\n"
    code += f"for(size_t index = 0; index < len; index++) {{\n"
    code += generate_switch(occurence_map)
    code += "}\ndest[len] = 0;\n return len + 1;\n}"
    return code

def generate_switch(alphabet_map):
    code = ""
    code += "switch(id | (((uint16_t)index) << 8)) {\n"

    alphabet = list(alphabet_map.keys())
    random.shuffle(alphabet)
    for c in alphabet:
        for v in alphabet_map[c]:
            code += f"case {v}: "
        if c == "\\":
            c = "\\\\"
        code += f"dest[index] = '{c}'; break;\n"
    code += "default: return 0;\n}\n"
    return code

def generate_header_code(filename, word_map):
    code = "#include <stdint.h>\n\n"
    for word_symbol in word_map:
        code += f"#define STRING_{word_symbol} ({word_map[word_symbol][0]}) // {word_map[word_symbol][1]}\n"
    code += "\nsize_t get_string_length(uint16_t id);\n"
    code += "\nsize_t get_string(uint16_t id, char * dest, size_t destSize);\n"
    return util.safeguard_code(code, filename)

def generate_source_code(header_name, word_map):
    alphabet_map = generate_alphabet_map(word_map)
    logging.getLogger().info(f"Alphabet size: {len(alphabet_map)}")
    code = f"#include \"{header_name}\"\n#include \"functions.h\"\n#include \"utility.h\"\n\n"
    code += generate_function_get_word_length(word_map)
    code += "\n\n"
    code += generate_function_generate_word(alphabet_map)
    return code

if __name__ == "__main__":
    util.setup_logger()
    logger = logging.getLogger()

    parser = argparse.ArgumentParser("Generates a word generator in form of a C header and source file.")

    parser.add_argument("--string-file",
        metavar = "FILE",
        help = "Specify the file containing the strings which should be possible to generate.",
        required = True)
    parser.add_argument("--header-path",
        metavar = "DIR",
        help = "Specify the header path",
        required = True)
    parser.add_argument("--source-path",
        metavar = "DIR",
        help = "Specify the source path",
        required = True)
    args = parser.parse_args()

    HEADER_NAME = os.path.basename(args.header_path)

    word_map = read_words(args.string_file)
    logger.info(f"Word amount: {len(word_map)}")

    header_code = generate_header_code(HEADER_NAME, word_map)
    source_code = generate_source_code(HEADER_NAME, word_map)

    with open(args.header_path, "w") as f:
        f.write(header_code)
        logger.info(f"Generated {args.header_path}")

    with open(args.source_path, "w") as f:
        f.write(source_code)
        logger.info(f"Generated {args.source_path}")
