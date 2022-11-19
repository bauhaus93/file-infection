import logging
import re
import subprocess

from instruction import Instruction

_logger = logging.getLogger(__name__)


def disassemble_file(filename: str) -> str:
    """
    Disassembles the given file with ndisasm/32-bit and
    annotates it with addresses being function entry points

    :param filename: Path to a ndisasm generated file with disassembly
    :returns: The file content, extended with function annotations
    """
    result = subprocess.run(
        ["ndisasm", "-b32", filename], capture_output=True, check=True
    )
    if result.returncode != 0:
        _logger.error(
            "Could not disassemble file '%s': %s",
            filename,
            result.stderr.decode("utf-8"),
        )
        return None
    return result.stdout.decode("utf-8")


def parse_file(filename: str, file_type: str) -> list[Instruction]:
    if file_type == "binary":
        return parse_disassembly(disassemble_file(filename))
    elif file_type == "disasm":
        with open(filename, "r") as f:
            return parse_disassembly(f.read())
    else:
        raise NotImplementedError(
            f"File parsing for type '{file_type}' is not supported!"
        )


_FILTER_WORDS = [
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


_DISASM_PATTERNS = {
    "ndisasm": re.compile(
        r"(?P<address>[0-9A-F]+)\s+(?P<bytecode>[0-9A-F]+)\s+(?P<instruction>\w+)(\s+(?P<args>.*))?",
        re.IGNORECASE,
    ),
    "objdump": re.compile(
        r"\s*(?P<address>[0-9A-F]+):\s+(?P<bytecode>[0-9A-F ]+)\s*(?P<instruction>\w+)(\s+(?P<args>.*))?",
        re.IGNORECASE,
    ),
}


def parse_disassembly(disassembly: str) -> list[dict[str, str]]:
    instructions = []
    matched_parsers = set()
    for line in disassembly.split("\n"):
        for name, pat in _DISASM_PATTERNS.items():
            match = pat.match(line)
            if match:
                instr = Instruction.from_match(match)
                instructions.append(instr)
                matched_parsers.add(name)
                break
    if len(matched_parsers) > 1:
        _logger.warning(
            "Multiple parsers matched for disassembly: %s",
            ", ".join(matched_parsers),
        )
    else:
        _logger.info("Parsed disassembly with parser '%s'", min(matched_parsers))
    return instructions
