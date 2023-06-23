import re
from .ids import next_name, with_cmd, break_cmd, firsts, with_stmt_line, stmt_add_brc, ret, refTo, WORD


class Structure:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_if(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.match(r"(\s*)else\s*:", line)
            if tmp is None:
                tmp = re.match(r"(\s*)((if|elif)(_not)?)\s+(.+):", line)
                if tmp is None:
                    result.append(line)
                else:
                    result.append(with_stmt_line(tmp.group(1),tmp.group(5)))
                    result.append(tmp.group(1) + tmp.group(2) + ":")
            else:
                result.append("%s %s" % (tmp.group(1) + with_cmd, "true"))
                result.append(tmp.group(1) + "elif:")
        self.content = "\n".join(result)
    
    def scan_for(self):
        result = list()
        for_name:tuple[str,str,str,str|None] = None
        outer_indent = inner_indent = None
        succeeded = False
        for line in self.content.split("\n"):
            if for_name is None:
                tmp = re.fullmatch(rf"(\s*)for\s+(.+?)\s*,\s*(.+?)\s*,\s*(.+?)\s*:\s*", line)
                if tmp is not None:
                    outer_indent = tmp.group(1)
                    for_name =  (tmp.group(2), tmp.group(3), tmp.group(4), None)
                tmp = re.fullmatch(rf"(\s*)for\s+({WORD})\s+in\s+(.+?):\s*", line)
                if tmp is not None:
                    outer_indent = tmp.group(1)
                    tmp_name = next_name("FOR")
                    result.append(outer_indent + tmp_name + " = " + tmp.group(3))
                    for_name = (tmp.group(2) + " = call " + tmp_name + ".beg",
                                "call " + tmp_name + ".cond",
                                tmp.group(2) + " = call " + tmp_name + ".next",
                                tmp_name)
                if for_name is not None:
                    succeeded = True
                    result.append(outer_indent + for_name[0])
                    result.append(outer_indent + "while " + for_name[1] + ":")
                    continue
                result.append(line)
            else:
                cur_indent = firsts(line)
                if inner_indent is None:
                    inner_indent = cur_indent
                else:
                    if len(cur_indent) < len(inner_indent):
                        result.append(inner_indent + for_name[2])
                        if for_name[3] is not None:
                            result.append(outer_indent + "~" + for_name[3])
                        for_name = None
                        outer_indent = inner_indent = None
                result.append(line)
        self.content = "\n".join(result)
        return succeeded

    def scan_all_for(self):
        while self.scan_for():
            ...
                    
                

    def scan_while(self):
        result = list()
        while_name = None
        stack = list()
        outer_indent = inner_indent = None
        for line in self.content.split("\n"):
            if while_name is False:
                result.append(line)
            elif while_name is not None:
                stack.append(line)
                if inner_indent is None:
                    inner_indent = firsts(line)
                elif len(firsts(line)) < len(inner_indent):
                    stack.pop()
                    stack[0] = outer_indent + "if:" + stack[0].strip()
                    stack.insert(0, with_stmt_line(outer_indent, while_name))
                    stack.append("%s %s" % (inner_indent +
                                            break_cmd, stmt_add_brc(while_name)))

                    result.extend(stack)
                    result.append(line)
                    while_name = False
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

    def scan_extern(self):
        result = list()
        indent = -1
        externals = list()
        succeeded = False
        for line in self.content.split("\n"):
            if indent != -1:
                cur_indent = len(firsts(line))
                if cur_indent < indent:
                    indent = -1
                    externals.clear()
                elif cur_indent == indent:
                    for name in externals:
                        line = re.sub(r"([^\w.])%s(\b)" % name, r"\1&%s\2" % name, line)
                result.append(line)
            else:
                tmp = re.match(r"(\s*)ext\s+(.*)", line)
                if tmp is None:
                    result.append(line)
                else:
                    indent = tmp.group(1)
                    for name in tmp.group(2).split(" "):
                        if len(name) > 0 and not name.isspace():
                            externals.append(name)
                    indent = len(indent)
                    succeeded = True
        self.content = "\n".join(result)
        return succeeded

    def scan_all_extern(self):
        self.content = re.sub(rf"(\s*)ext\s+({WORD})\s*=\s*(.+)", r"\1ext \2\1\2 = \3", self.content)
        while self.scan_extern():
            ...

    def scan_returns(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.match(rf"(\s*){ret}\s+(.+)", line)
            if tmp is None:
                result.append(line)
            else:
                result.append("%s %s" % (tmp.group(1) + with_cmd,
                                         stmt_add_brc(tmp.group(2))))
                result.append(tmp.group(1) + ret)
        self.content = "\n".join(result)

    def break_shortcuts(self):
        self.content = re.sub(
            r"break(\s*\n)", break_cmd + r" false\1", self.content)
        self.content = re.sub(
            r"conti(\s*\n)", break_cmd + r" true\1", self.content)

    def work(self, output_name: str) -> str:
        self.scan_if()
        self.scan_all_for()
        self.scan_all_while()
        self.scan_all_extern()
        self.scan_returns()
        self.break_shortcuts()
        output = output_name + ".struct"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
