import re

from .ids import next_name, with_cmd, moveTo, WORD


class OperatorObject:
    value: str

    def __init__(self, value):
        self.value = value

    def __add__(self, other):
        return OperatorObject("(%s.add %s)" % (self.value, other.value))

    def __sub__(self, other):
        return OperatorObject("(%s.sub %s)" % (self.value, other.value))

    def __mul__(self, other):
        return OperatorObject("(%s.mul %s)" % (self.value, other.value))

    def __div__(self, other):
        return OperatorObject("(%s.div %s)" % (self.value, other.value))

    def __lt__(self, other):
        return OperatorObject("(%s.lt %s)" % (self.value, other.value))

    def __le__(self, other):
        return OperatorObject("(%s.le %s)" % (self.value, other.value))

    def __gt__(self, other):
        return OperatorObject("(%s.gt %s)" % (self.value, other.value))

    def __ge__(self, other):
        return OperatorObject("(%s.ge %s)" % (self.value, other.value))

    def __eq__(self, other):
        return OperatorObject("(%s.eq %s)" % (self.value, other.value))

    def __ne__(self, other):
        return OperatorObject("(%s.ne %s)" % (self.value, other.value))

    def __and__(self, other):
        return OperatorObject("(%s.and %s)" % (self.value, other.value))

    def __or__(self, other):
        return OperatorObject("(%s.or %s)" % (self.value, other.value))


def analyze(s: str) -> str:
    loc = locals()
    changed = dict()
    names = re.findall(rf"{WORD}", s)
    for name in names:
        if re.fullmatch(r"\w+", name) is None:
            change_to = next_name("OP_TMP")
            changed[change_to] = name
            s = s.replace(name, change_to)
            name = change_to
        loc[name] = OperatorObject(name)
    result: OperatorObject = eval(s)
    value = result.value
    for change_to, name in changed.items():
        value = value.replace(change_to, name)
    return value


class Operators:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_operators(self):
        result = str()
        for line in self.content.split("\n"):
            tmp = re.search(r"\{expr\s+(.+?)}", line)
            if tmp is None:
                result += line + "\n"
            else:
                result += line[:tmp.start(0)] + analyze(tmp.group(1)) + line[tmp.end(0):] + "\n"
        self.content = result

    def scan_pairs(self):
        self.content = re.sub(rf"({WORD})\s*::\s*({WORD})", r"(pair \1 \2)", self.content)
        
        
    def scan_ranges(self):
        self.content = re.sub(rf"({WORD})\s*->\s*({WORD})\s*\|\s*({WORD})", r"(range \1 \2 \3)", self.content)
        self.content = re.sub(rf"({WORD})\s*->\s*({WORD})", r"(range \1 \2)", self.content)

    def work(self, output_name: str):
        self.scan_operators()
        self.scan_pairs()
        self.scan_ranges()
        output = output_name + ".no_op"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
