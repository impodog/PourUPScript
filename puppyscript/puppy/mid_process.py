import re

from .ids import firsts


class MidProcess:
    content: str

    def __init__(self, file: str):
        self.commands = list()
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def fix_inline_block(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(r"(.*?[^:]:)([^:].*)", line)
            if tmp is None or tmp.group(2).isspace():
                result.append(line)
            else:
                result.append(tmp.group(1))
                result.append("\t" + tmp.group(2))
        self.content = "\n".join(result)

    def fix_line_multi_statement(self):
        result = list()
        for line in self.content.split("\n"):
            statements = line.split(';')
            length = len(statements)
            if length == 1:
                result.append(line)
            else:
                indent = firsts(statements[0].rstrip())
                for i in range(1, length):
                    statements[i] = indent + statements[i].lstrip()
                result.extend(statements)
        self.content = "\n".join(result)

    def work(self, output_name: str) -> str:
        self.fix_inline_block()
        self.fix_line_multi_statement()
        output = output_name + ".midp"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
