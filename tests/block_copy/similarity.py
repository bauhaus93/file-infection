import logging

from disassembly import filter_disassembly

_logger = logging.getLogger(__name__)


def verify_instruction_similarity(disassembly_original, disassembly_modified):
    unmod_dict = filter_disassembly(disassembly_original)
    mod_dict = filter_disassembly(disassembly_modified)

    total = 0
    diff_count = 0
    new_appeared = 0
    new_missing = 0
    for key in set([*unmod_dict.keys(), *mod_dict.keys()]):
        unmod_count = unmod_dict.get(key, 0)
        mod_count = mod_dict.get(key, 0)
        if unmod_count != mod_count:
            _logger.error(
                "Amount of instruction occurences changed | %40s | original: %4d | copied: %4d",
                key,
                unmod_count,
                mod_count,
            )
            if mod_count > unmod_count:
                new_appeared += mod_count - unmod_count
            elif mod_count < unmod_count:
                new_missing += unmod_count - mod_count
            diff_count += abs(unmod_count - mod_count)
            total += unmod_count

    _logger.error("Newly appeared instructions: %d", new_appeared)
    _logger.error("Newly missing instructions: %d", new_missing)
    _logger.error("Total instruction difference: %d", diff_count)
    return diff_count
