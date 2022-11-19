import re


class Instruction(object):

    address = None
    bytecode = None
    instruction = None
    args = None

    _hex_pattern = re.compile(r"(0x)?[0-9A-F]+", re.IGNORECASE)

    @classmethod
    def from_match(cls, match):
        instr = cls()
        instr.address = int(match.group("address"), 16)
        instr.bytecode = match.group("bytecode")
        instr.instruction = match.group("instruction")
        instr.args = match.group("args")
        return instr

    @property
    def is_relative_call(self):
        return (
            self.instruction == "call"
            and self._hex_pattern.fullmatch(self.args) is not None
        )

    @property
    def is_nop(self):
        return self.instruction == "nop"

    def __str__(self):
        return " ".join([self.address, self.bytecode, self.instruction, self.args])
