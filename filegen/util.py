import os
import logging
import string


def setup_logger():
    FORMAT = r"[%(asctime)-15s] %(levelname)s - %(message)s"
    DATE_FORMAT = r"%Y-%m-%d %H:%M:%S"
    logging.basicConfig(level=logging.INFO, format=FORMAT, datefmt=DATE_FORMAT)


def create_safeguard(filename):
    allowed_chars = string.ascii_letters + string.digits + "_"
    safeguard = "".join(
        c if c in allowed_chars else "_" for c in os.path.basename(filename).upper()
    )
    if safeguard[0] in string.digits:
        safeguard = safeguard[1:]
    return safeguard


def safeguard_code(code, filename):
    sg = create_safeguard(filename)
    return f"#ifndef {sg}\n#define {sg}\n\n{code}\n\n#endif // {sg}"
