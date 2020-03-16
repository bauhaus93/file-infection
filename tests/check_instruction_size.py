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
	FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
	DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"
	logging.basicConfig(level = logging.DEBUG, format = FORMAT, datefmt = DATE_FORMAT)


class Disassembler:

	def __init__(self, start_address, lib_path):
		self.lib = ctypes.cdll.LoadLibrary(lib_path)

		self.lib.init_disasm.restype = ctypes.c_void_p
		self.lib.init_disasm.argtypes = [ctypes.c_void_p]

		self.lib.destroy_disasm.argtypes = [ctypes.c_void_p]

		self.lib.next_instruction.restype = ctypes.c_bool
		self.lib.next_instruction.argtypes = [ctypes.c_void_p]

		self.lib.get_instruction_size.restype = ctypes.c_ubyte
		self.lib.get_instruction_size.argtypes = [ctypes.c_void_p]

		self.disasm = self.lib.init_disasm(start_address)

	def __del__(self):
		try:
			if self.disasm:
				self.lib.destroy_disasm(ctypes.byref(self.disasm))
		except:
			pass

	def next_instruction(self):
		return bool(self.lib.next_instruction(self.disasm))

	def get_instruction_size(self):
		return int(self.lib.get_current_instruction_size(ctypes.c_void_p(self.disasm)))


def check_instructions(file_path, bit_width):
	total = 0
	mismatches = 0
	with open(file_path, "r") as f:
		for line in f.readlines():
			line = line.rstrip()
			if len(line) == 0 or line[0] == ";":
				continue
			data = compile_instruction(line, bit_width)
			if data is None:
				return False
			if len(data) == 0:
				continue

			bin_str = " ".join([f"{v:02X}" for v in data])
			try:
				disasm = Disassembler(data, lib_path)
				if disasm.next_instruction():
					found_size = disasm.get_instruction_size()
					if len(data) != found_size:
						logger.error(f"Mismatch | {line:40s} | {bin_str:20s} | expected = {len(data)}, found = {found_size}")
						mismatches += 1
					else:
						logger.debug(f"Instruction | {line:40s} | {bin_str:20s} | {len(data):2d} byte")

			except Exception as e:
				logger.error(f"Exception during disassembly: {e}")
				return False


			total += 1
	logger.info(f"Instruction match finished:  {total - mismatches}/{total} OK")
	return mismatches == 0

def compile_instruction(instruction, bit_width):
	input_file = os.path.join(tempfile.gettempdir(), str(uuid.uuid4()) + ".s")
	with open(input_file, "w") as f:
		f.write(f"[bits {bit_width}]\n{instruction}")

	output_file = os.path.join(tempfile.gettempdir(),  str(uuid.uuid4()) + ".bin")
	try:
		result = subprocess.run(["nasm", "-o", output_file, input_file], capture_output = True)
	except FileNotFoundError as e:
		logger.error(f"subprocess.run: {e}")
		return None
	if result.returncode != 0:
		logger.error(f"Could not compile instruction '{instruction}'")
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
	parser = argparse.ArgumentParser(description = "Compiles each line from the asm file, then checks the compiled size against the size determined by the disassembly library.")
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
	if check_instructions(source_path, args.bit_width):
		exit(0)
	else:
		exit(1)
