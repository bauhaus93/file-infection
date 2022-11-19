import logging

from disassembly import get_instruction_count

_logger = logging.getLogger(__name__)


def verify_instruction_similarity(disassembly_original, disassembly_modified):
    unmod_dict = get_instruction_count(disassembly_original)
    mod_dict = get_instruction_count(disassembly_modified)

    new_appeared = 0
    new_missing = 0
    for key in set([*unmod_dict.keys(), *mod_dict.keys()]):
        unmod_count = unmod_dict.get(key, 0)
        mod_count = mod_dict.get(key, 0)
        if unmod_count != mod_count:
            _logger.error(
                "Amount of instruction occurences changed "
                "| %10s | original: %4d | copied: %4d | delta: %4d",
                key,
                unmod_count,
                mod_count,
                mod_count - unmod_count,
            )
            if mod_count > unmod_count:
                new_appeared += mod_count - unmod_count
            elif mod_count < unmod_count:
                new_missing += unmod_count - mod_count

    _logger.error("Instruction delta: +%d/-%d", new_appeared, new_missing)
    return new_appeared + new_missing
