import re

from .ids import next_name, targeting, moveTo


class OperatorObject:
    value: str

    def __init__(self, value):
        self.value = value

    def __add__(self, other):
        return OperatorObject("(add %s %s)" % (self.value, other.value))

    def __sub__(self, other):
        return OperatorObject("(sub %s %s)" % (self.value, other.value))

    def __mul__(self, other):
        return OperatorObject("(mul %s %s)" % (self.value, other.value))

    def __div__(self, other):
        return OperatorObject("(div %s %s)" % (self.value, other.value))

    def __lt__(self, other):
        return OperatorObject("(lt %s %s)" % (self.value, other.value))

    def __le__(self, other):
        return OperatorObject("(le %s %s)" % (self.value, other.value))

    def __gt__(self, other):
        return OperatorObject("(gt %s %s)" % (self.value, other.value))

    def __ge__(self, other):
        return OperatorObject("(ge %s %s)" % (self.value, other.value))

    def __eq__(self, other):
        return OperatorObject("(eq %s %s)" % (self.value, other.value))

    def __ne__(self, other):
        return OperatorObject("(ne %s %s)" % (self.value, other.value))

    def __and__(self, other):
        return OperatorObject("(and %s %s)" % (self.value, other.value))

    def __or__(self, other):
        return OperatorObject("(or %s %s)" % (self.value, other.value))


def analyze(s: str) -> str:
    loc = locals()
    names = re.findall(r"\w+", s)
    for name in names:
        loc[name] = OperatorObject(name)
    result: OperatorObject = eval(s)
    return result.value


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

    def work(self, output_name: str):
        self.scan_operators()
        output = output_name + ".no_op"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
