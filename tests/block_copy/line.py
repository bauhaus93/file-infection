import logging

_logger = logging.getLogger(__name__)


def create_linewise_comparison(unmodified_file, modified_file):
    output = ""
    for unmod, mod in zip(unmodified_file.split("\n"), modified_file.split("\n")):
        output += f"{unmod:60}\t|\t{mod:60}".strip() + "\n"

    return output
