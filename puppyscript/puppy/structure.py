import re
from .ids import targeting, ret, firsts, is_word


def target_line(indent: str, var: str) -> str:
    if is_word(var):
        return indent + targeting + " " + var
    else:
        return indent + var


class Structure:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_if(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.match(r"(\s*)if\s+(.+):", line)
            if tmp is None:
                result.append(line)
            else:
                if is_word(tmp.group(2)):
                    fmt = "%s %s"
                else:
                    fmt = "%s (%s)"
                result.append(fmt % (tmp.group(1) + targeting, tmp.group(2)))
                result.append(tmp.group(1) + "if:")
        self.content = "\n".join(result)

    def scan_while(self):
        result = list()
        while_name = None
        stack = list()
        outer_indent = inner_indent = None
        for line in self.content.split("\n"):
            if while_name is not None:
                stack.append(line)
                if inner_indent is None:
                    inner_indent = firsts(line)
                elif len(firsts(line)) < len(inner_indent):
                    stack.pop()
                    stack[0] = outer_indent + "if:" + stack[0].strip()
                    stack.insert(0, target_line(outer_indent, while_name))
                    stack.append(target_line(inner_indent, while_name))
                    stack.append(inner_indent + ret)

                    result.extend(stack)
                    result.append(line)
                    break
            else:
                tmp = re.match(r"(\s*)while\s+(.+):", line)
                if tmp is None:
                    result.append(line)
                else:
                    while_name = tmp.group(2)
                    outer_indent = tmp.group(1)
                    stack = [outer_indent + "while:"]

        self.content = "\n".join(result)
        return while_name is not None

    def scan_all_while(self):
        while self.scan_while():
            ...

    def work(self, output_name: str) -> str:
        self.scan_if()
        self.scan_all_while()
        output = output_name + ".struct"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
