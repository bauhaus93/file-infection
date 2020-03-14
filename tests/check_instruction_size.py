#!/bin/env python3

import argparse
import os
import logging
import subprocess
import tempfile
import uuid
import ctypes

logger = logging.getLogger()

def setup_logger():
	FORMAT = r"[%(asctime)-15s] %(levelname)s - %(message)s"
	DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"
	logging.basicConfig(level = logging.INFO, format = FORMAT, datefmt = DATE_FORMAT)

class Disassembler:

	def __init__(self, lib_path):
		self.lib = ctypes.cdll.LoadLibrary(lib_path)
	
	def disasm(self, memory):
		self.lib.disasm(ctypes.c_void_p(memory))
			

def check_instructions(file_path, bit_width):
	total = 0
	errors = []
	with open(file_path, "r") as f:
		for line in f.readlines():
			line = line.rstrip()
			if len(line) == 0 or line[0] == ";":
				continue
			data = compile_instruction(line, bit_width)
			logger.info(f"instruction | {line:30s} | size = {len(data):2d} byte")
			total += 1
	logger.info(f"Checked {total} instructions, {len(errors)} errors")
	return len(errors) == 0

def compile_instruction(instruction, bit_width):
	input_file = os.path.join(tempfile.gettempdir(), str(uuid.uuid4()) + ".s")
	with open(input_file, "w") as f:
		f.write(f"[bits {bit_width}]\n{instruction}")

	output_file = os.path.join(tempfile.gettempdir(),  str(uuid.uuid4()) + ".bin")
	result = subprocess.run(["nasm", "-o", output_file, input_file], capture_output = True)
	if result.returncode != 0:
		logger.error(f"Could not compile instruction")
		for line in str(result.stdout).split("\\n"):
			if len((line[2:])) > 0:
				logger.error("stdout: " + line[2:])

		for line in str(result.stderr).split("\\n"):
			if len((line[2:])) > 0:
				logger.error("stderr: " + line[2:])
		os.remove(input_file)
		return None
	with open(output_file, "rb") as f:
		data = f.read()
	os.remove(input_file)
	os.remove(output_file)
	return data

if __name__ == "__main__":
	setup_logger()
	parser = argparse.ArgumentParser(description = "Compiles the given asm file, then checks the size of the instructions with the size given in the source file as a comment.")
	parser.add_argument('source_file', metavar = "FILE", type = str, help = "File for which to check instruction sizes")
	parser.add_argument('--bit-width', choices = [32, 64], type = int, default = 32, help = "Set the bit width used for compilation of the instructions.")
	parser.add_argument('--disasm-lib', metavar = "FILE", type = str, required = True, help = "Disassembler libary to be checked")
	args = parser.parse_args()

	source_path = os.path.abspath(args.source_file)
	if not os.path.isfile(source_path):
		logger.error(f"Could not find source file '{source_path}'")
		exit(1)

	lib_path = os.path.abspath(args.disasm_lib)
	if not os.path.isfile(lib_path):
		logger.error(f"Could not find disassembler libary '{lib_path}')")
		exit(1)

	disasm = Disassembler(lib_path)

	if check_instructions(source_path, args.bit_width):
		exit(0)
	else:
		exit(1)


	
