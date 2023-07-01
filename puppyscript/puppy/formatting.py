import re

from .ids import WORD


class Formatting:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_pairs(self):
        self.content = re.sub(rf"({WORD})\s*::\s*({WORD})", r"(pair \1 \2)", self.content)

    def scan_ranges(self):
        self.content = re.sub(rf"({WORD})\s*->\s*({WORD})\s*\|\s*({WORD})", r"(range \1 \2 \3)", self.content)
        self.content = re.sub(rf"({WORD})\s*->\s*({WORD})", r"(range \1 \2)", self.content)


    def work(self, output_name: str):
        self.scan_pairs()
        self.scan_ranges()
        output = output_name + ".form"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
