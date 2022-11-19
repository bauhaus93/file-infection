import re


class Instruction(object):

    label = None
    address = None
    bytecode = None
    instruction = None
    args = None
    destination_name = None

    _hex_pattern = re.compile(r"(0x)?[0-9A-F]+", re.IGNORECASE)
    _named_target_pattern = re.compile(
        r"(0x)?(?P<target_offset>[0-9A-F]+)\s*<(?P<target_name>[^<>]+)>", re.IGNORECASE
    )

    @classmethod
    def from_match(cls, match):
        instr = cls()
        instr.address = int(match.group("address").strip(), 16)
        instr.bytecode = match.group("bytecode").strip()
        instr.instruction = match.group("instruction").strip()
        if match.group("args"):
            instr.args = match.group("args").strip()
            dest_match = cls._named_target_pattern.search(instr.args)
            if dest_match:
                instr.args = dest_match.group("target_offset").strip()
                instr.destination_name = dest_match.group("target_name").strip()

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
        return " ".join(
            filter(
                lambda e: e is not None,
                [
                    self.address,
                    self.bytecode,
                    self.instruction,
                    self.args,
                    f"@ {self.destination_name}" if self.destination_name else None,
                ],
            )
        )


def count_different_instructions(instructions: list[Instruction]) -> dict[str, int]:
    instruction_count = {}
    for instr in instructions:
        if not instr.is_nop:
            instruction_count[instr.instruction] = (
                instruction_count.get(instr.instruction, 0) + 1
            )
    return instruction_count


def get_call_targets(instructions: list[Instruction]) -> dict[int, int]:
    targets = {}
    for instr in filter(lambda i: i.is_relative_call, instructions):
        dest = int(instr.args, 16)
        targets[dest] = targets.get(dest, 0) + 1
    return targets
