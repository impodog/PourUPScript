import re

from .ids import is_debug_info

class ExprOptim:
    content: str
    debug_mode: bool

    def __init__(self, file: str, debug_mode: bool):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()
        self.debug_mode = debug_mode

    def remove_too_many_info(self):
        if self.debug_mode:
            result = list()
            prev = False
            for line in self.content.split("\n"):
                if is_debug_info(line):
                    if not prev:
                        result.append(line)
                    prev = True
                else:
                    result.append(line)
                    prev = False
            self.content = "\n".join(result)

    def work(self, output_name: str) -> str:
        self.remove_too_many_info()
        output = output_name + ".expr_opt"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
