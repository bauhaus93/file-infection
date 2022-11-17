import logging

_logger = logging.getLogger(__name__)


def create_linewise_comparison(unmodified_file, modified_file):
    for unmod, mod in zip(unmodified_file.split("\n"), modified_file.split("\n")):
        print(f"{unmod:80}\t{mod:80}")
