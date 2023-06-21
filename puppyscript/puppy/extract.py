import re
from .ids import next_name


def need_add_backslash(c: str):
    return c.isspace() or c == ":" or c == "\\"


class Extract:
    extracted: dict[str, object]
    content: str
    result: str

    def __init__(self, file: str):
        self.extracted = dict()
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()
        self.result = str()

    def scan_constexpr(self):
        while True:
            result = re.search(r"\{constexpr\s+(.+?)}", self.content)
            if result is None:
                break
            try:
                self.content = self.content[:result.start(0)] + str(eval(result.group(1))) + self.content[
                                                                                             result.end(0):]
            except:
                raise SyntaxError("Cannot analyze constexpr \"%s\"." % result.group(1))

    def extract_number(self):
        while True:
            result = re.search(r"(.)(\d+)(\.\d+)?\b", self.content)
            if result is None or result.group(1).isalnum():
                break
            name = next_name("NUM")
            if result.group(3) is None:
                value = result.group(2)
            else:
                value = result.group(2) + result.group(3)
            is_negative = result.group(1) == "-"
            if is_negative:
                value = "-" + value
            self.extracted[name] = eval(value)
            self.content = re.sub(((r'([^.\w])' if is_negative else r'([^.\-\w])') + r"%s(\b)") % value,
                                  r"\1%s\2" % name,
                                  self.content)

    def extract_string(self):
        self.content = " " + self.content
        while True:
            result = re.search(r'[^\\]"(.*?[^\\])"', self.content)
            if result is None:
                break
            name = next_name("STR")
            value = result.group(1)
            tmp = value
            if len(tmp) > 0:
                if need_add_backslash(tmp[0]):
                    tmp = "\\\\" + tmp
                if need_add_backslash(tmp[-1]):
                    tmp += "\\\\"
            self.extracted[name] = eval('"' + tmp + '"').replace("\n", "\\n")
            self.content = self.content.replace('"%s"' % value, name, 1)
        self.content = self.content[1:]

    def make_extraction(self):
        def removes():
            nonlocal self, to_remove
            for k in to_remove:
                self.extracted.pop(k)
            to_remove.clear()

        def extract(segment: str, ty: type):
            nonlocal self, to_remove
            self.result += segment + ":\n"
            for key, value in self.extracted.items():
                if isinstance(value, ty):
                    self.result += key + " = " + str(value) + "\n"
                    to_remove.append(key)
            removes()

        to_remove: list[str] = list()
        extract("INT", int)
        extract("FLOAT", float)
        extract("STR", str)

    def work(self, output_name: str) -> str:
        self.scan_constexpr()
        self.extract_string()
        self.extract_number()
        self.make_extraction()

        output = output_name + ".extract"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)

        with open(output_name + ".con", "w", encoding="utf-8") as f:
            f.write(self.result)
        return output
