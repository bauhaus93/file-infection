import os
import logging
import subprocess
import tempfile
import uuid
import ctypes
import threading

logger = logging.getLogger()


class Disassembler:
    def __init__(self, start_address, lib_path):
        self.lib = ctypes.cdll.LoadLibrary(lib_path)

        self.lib.init_disasm.restype = ctypes.c_void_p
        self.lib.init_disasm.argtypes = [ctypes.c_void_p]

        self.lib.destroy_disasm.argtypes = [ctypes.c_void_p]

        self.lib.next_instruction.restype = ctypes.c_bool
        self.lib.next_instruction.argtypes = [ctypes.c_void_p]

        self.lib.get_current_instruction_size.restype = ctypes.c_ubyte
        self.lib.get_current_instruction_size.argtypes = [ctypes.c_void_p]

        #self.lib.print_current_instruction.restype = ctypes.c_void
        self.lib.print_current_instruction.argtypes = [ctypes.c_void_p]

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

    def print_instruction(self):
        self.lib.print_current_instruction(ctypes.c_void_p(self.disasm))


def setup_logger():
    FORMAT = r"[%(asctime)-15s %(levelname)-5s] %(message)s"
    DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"
    logging.basicConfig(level=logging.INFO, format=FORMAT, datefmt=DATE_FORMAT)


def check_instructions(file_path, bit_width, lib_path):
    total = 0
    mismatches = 0
    with open(file_path, "r") as f:
        for line in f.readlines():

            line = line.rstrip()
            if len(line) == 0 or line[0] == ";":
                continue
            data = compile_instruction(line, bit_width, silent_error=False)
            if data is None:
                return False

            bin_str = " ".join([f"{v:02X}" for v in data])
            try:
                disasm = Disassembler(data, lib_path)
                if disasm.next_instruction():
                    found_size = disasm.get_instruction_size()
                    if len(data) != found_size:
                        logger.error(
                            f"Mismatch    | {line:40s} | {bin_str:20s} | expected = {len(data):2d} | found = {found_size:2d}"
                        )
                        disasm.print_instruction()
                        mismatches += 1
                    else:
                        logger.debug(
                            f"Instruction | {line:40s} | {bin_str:20s} | {len(data):2d} byte"
                        )
                else:
                    mismatches += 1
                    logger.error(
                        f"Mismatch    | {line:40s} | {bin_str:20s} | expected valid instruction, found invalid"
                    )

            except Exception as e:
                logger.error(f"Exception during disassembly: {e}")
                return False

            total += 1
    logger.info(f"Instruction match finished:  {total - mismatches}/{total} OK")
    return mismatches == 0


def validate_instructions_binary(instructions, bit_width):
    input_file = os.path.join(
        tempfile.gettempdir(), str(os.getpid()) + "_" + str(uuid.uuid4()) + ".s"
    )
    with open(input_file, "w") as f:
        f.write(f"[bits {bit_width}]\n" + "\n".join(instructions))

    output_file = os.path.join(
        tempfile.gettempdir(), str(os.getpid()) + "_" + str(uuid.uuid4()) + ".bin"
    )
    result = subprocess.call(["nasm", "-o", output_file, input_file], stdout = subprocess.DEVNULL, stderr= subprocess.DEVNULL)
    os.remove(input_file)
    if result == 0:
        os.remove(output_file)
        return instructions
    elif len(instructions) == 1:
        return []
    else:
        left = instructions[:len(instructions)//2]
        right = instructions[len(instructions)//2:]

        valid = []
        valid.extend(validate_instructions_binary(left, bit_width))
        valid.extend(validate_instructions_binary(right, bit_width))
        return valid

def compile_instruction(instruction, bit_width, silent_error=False):
    input_file = os.path.join(
        tempfile.gettempdir(), str(os.getpid()) + "_" + str(threading.current_thread().ident) + "_" + str(uuid.uuid4()) + ".s"
    )
    with open(input_file, "w") as f:
        f.write(f"[bits {bit_width}]\n{instruction}")

    output_file = os.path.join(
        tempfile.gettempdir(), str(os.getpid()) + "_" + str(uuid.uuid4()) + ".bin"
    )
    try:
        result = subprocess.run(
            ["nasm", "-o", output_file, input_file], capture_output=True
        )
    except FileNotFoundError as e:
        logger.error(f"subprocess.run: {e}")
        return None
    if result.returncode != 0:
        if not silent_error:
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
    if len(data) == 0:
        logger.error(f"Assembly successful, but output file is empty for '{instruction}'")
        return None
    return data
