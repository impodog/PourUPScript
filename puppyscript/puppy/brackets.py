import re
from .ids import next_name, firsts, temp_id, targeting, moveTo


class Brackets:
    content: str
    result: str

    def __init__(self, file: str):
        with open(file, "r") as f:
            self.content = f.read()
        self.result = str()

    def put_line(self, line: str):
        if len(line) > 0:
            self.result += line + "\n"

    def scan_bracket(self, s: str, indent: str = None):
        count = 0
        begin = -1
        i = 0
        if indent is None:
            indent = firsts(s)
        s = s.strip()
        for c in s:
            match c:
                case '(':
                    count += 1
                    if begin == -1:
                        begin = i
                case ')':
                    count -= 1
            if begin != -1:
                if count == 0:
                    tmp = s[begin + 1:i]
                    name = next_name("BRC")
                    self.scan_bracket(tmp, indent)
                    self.put_line("%s %s" % (indent + moveTo, name))
                    tmp = s[:begin] + name + s[i + 1:]
                    if tmp.strip() != name:
                        self.scan_bracket(tmp, indent)
                    return
            i += 1
        self.put_line(indent + s)

    def scan(self):
        for line in self.content.split("\n"):
            self.scan_bracket(line)

    def work(self, output_name: str) -> str:
        self.scan()
        output = output_name + ".no_brc"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.result)
        return output
