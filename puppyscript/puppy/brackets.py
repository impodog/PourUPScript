import re
from .ids import next_name, firsts, moveTo, WORD


class Brackets:
    content: str
    result: str
    to_delete: list[str]

    def __init__(self, file: str):
        with open(file, "r") as f:
            self.content = f.read()
        self.result = str()
        
    def remove_redundant_brackets(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch("(\s*)\((.+)\)\s*", line)
            if tmp is None:
                result.append(line)
            else:
                result.append(tmp.group(1) + tmp.group(2))
        self.content = "\n".join(result)
                

    def put_line(self, line: str):
        if len(line) > 0:
            self.result += line + "\n"
    
    def reset(self):
        self.to_delete = list()

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
                    if begin > 0 and re.fullmatch(rf".+{WORD}", s[:begin]):
                        begin = -1
                    else:
                        tmp = s[begin + 1:i]
                        name = next_name("FAC")
                        self.to_delete.append(name)
                        self.scan_bracket(tmp, indent)
                        self.put_line("%s %s" % (indent + moveTo, name))
                        tmp = s[:begin] + name + s[i + 1:]
                        if tmp.strip() != name:
                            self.scan_bracket(tmp, indent)
                        return indent
            i += 1
        self.put_line(indent + s)

    def scan(self):
        for line in self.content.split("\n"):
            self.reset()
            indent = self.scan_bracket(line)
            for name in self.to_delete:
                self.put_line(indent + "~" + name)

    def work(self, output_name: str, time: int) -> str:
        self.remove_redundant_brackets()
        self.scan()
        output = output_name + ".no_brc" + str(time)
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.result)
        return output

