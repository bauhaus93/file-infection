import logging
import re

_logger = logging.getLogger(__name__)


def verify_call_targets(disassembly_original, disassembly_modified):
    orig_targets = collect_call_targets(disassembly_original)
    mod_targets = collect_call_targets(disassembly_modified)

    targets = set([*orig_targets.keys(), *mod_targets.keys()])
    diff_count = 0
    total = 0
    for target in targets:
        c_orig = orig_targets.get(target, 0)
        c_mod = mod_targets.get(target, 0)
        if c_orig == 0:
            _logger.error("New call\t\t| amount: %3d| target:  0x%08X", c_mod, target)
        if c_mod == 0:
            _logger.error(
                "Missing call\t| amount: %3d| target:  0x%08X", c_orig, target
            )
        diff_count += abs(c_orig - c_mod)
        total += c_orig + c_mod
    return diff_count


def collect_call_targets(disassembly):
    pat = re.compile("call (0x[0-9a-fA-F]+)")
    pat = re.compile(
        r"(?P<src>[0-9A-F]+)\s+call (?P<target_address>0x[0-9A-F]+)",
        re.IGNORECASE,
    )
    targets = {}
    for match in pat.finditer(disassembly):
        dest = int(match.group("target_address"), 16)
        targets[dest] = targets.get(dest, 0) + 1
    return targets
