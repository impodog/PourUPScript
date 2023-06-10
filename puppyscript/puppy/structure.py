import re
from .ids import with_cmd, break_cmd, firsts, with_stmt_line, stmt_add_brc


class Structure:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_if(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.match(r"(\s*)(if|if_not)\s+(.+):", line)
            if tmp is None:
                result.append(line)
            else:
                result.append("%s %s" % (tmp.group(1) + with_cmd, stmt_add_brc(tmp.group(3))))
                result.append(tmp.group(1) + tmp.group(2) + ":")
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
                    stack.insert(0, with_stmt_line(outer_indent, while_name))
                    stack.append("%s %s" % (inner_indent + break_cmd, stmt_add_brc(while_name)))

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

    def break_shortcuts(self):
        self.content = re.sub(r"break(\s*\n)", break_cmd + r" false\1", self.content)
        self.content = re.sub(r"conti(\s*\n)", break_cmd + r" true\1", self.content)

    def work(self, output_name: str) -> str:
        self.scan_if()
        self.scan_all_while()
        self.break_shortcuts()
        output = output_name + ".struct"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
