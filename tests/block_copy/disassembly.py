import logging
import re
import subprocess

from call import collect_call_targets

_logger = logging.getLogger(__name__)


def disassemble_file(filename):
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
    return annonate_disassembly(result.stdout.decode("utf-8"))


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


def annonate_disassembly(disassembly):
    output_lines = []
    pat = re.compile(r"(?P<addr>[0-9A-F]+)", re.IGNORECASE)
    call_targets = set(collect_call_targets(disassembly).keys())
    for line in disassembly.split("\n"):
        match = pat.match(line)
        if match:
            addr = int(match.group("addr"), 16)
            if addr in call_targets:
                output_lines.append(f"{'#'*24} Function {'#'*24}")
            output_lines.append(line)
    return "\n".join(output_lines)


def filter_disassembly(disassembly):
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
            lambda kv: not (any(map(lambda w: w in kv[0], _FILTER_WORDS))),
            diss_dict.items(),
        )
    )
