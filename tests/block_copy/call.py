import logging
import re

from instruction import Instruction, get_call_targets

_logger = logging.getLogger(__name__)


def verify_call_targets(
    instructions_original: list[Instruction], instructions_modified: list[Instruction]
) -> int:
    orig_targets = get_call_targets(instructions_original)
    mod_targets = get_call_targets(instructions_modified)

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
                "Missing call\t| amount: %3d | target:  0x%08X", c_orig, target
            )
        diff_count += abs(c_orig - c_mod)
        total += c_orig + c_mod
    return diff_count
