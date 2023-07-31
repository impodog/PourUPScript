import re

from .ids import with_cmd, break_cmd, firsts, stmt_add_brc, ret, WORD, FLAGS, word_last, spec_name_of, moveTo


class Syntactic:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

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

    def unmap_import(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(r"(\s*)(use|unmap)\s+import\s+(.+)", line)
            if tmp is None:
                result.append(line)
            else:
                result.append(tmp.group(1) + "import " + tmp.group(3))
                result.append(tmp.group(1) + "unmap")
        self.content = "\n".join(result)

    def scan_import(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(rf"(\s*)import\s+({WORD})\s+as\s+({WORD})", line)
            if tmp is None:
                result.append(line)
            else:
                result.append("%s %s" % (tmp.group(1) + "import", tmp.group(2)))
                result.append("%s %s" % (tmp.group(1) + moveTo, tmp.group(3)))
        self.content = "\n".join(result)

    def use_statements(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.match(rf"(\s*)use\s*({WORD})(\s+as\s+({WORD}))?\s*", line)
            if tmp is None:
                result.append(line)
            else:
                if tmp.group(4) is None:
                    result.append(tmp.group(1) + word_last(tmp.group(2)) + " = " + tmp.group(2))
                else:
                    result.append(tmp.group(1) + tmp.group(4) + " = " + tmp.group(2))
        self.content = "\n".join(result)

    def special_functions(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(rf"(\s*)spec\s+{FLAGS}(func|proc)\s+({WORD})\s*:\s*", line)
            if tmp is not None:
                line = tmp.group(1) + \
                       tmp.group(3) + " " + spec_name_of(tmp.group(4)) + ":"
            result.append(line)
        self.content = "\n".join(result)

    def array_indexes(self):
        result = list()
        success = False
        for line in self.content.split("\n"):
            tmp = re.search(rf"\b({WORD})\[({WORD})]", line)
            if tmp is not None:
                success = True
                line = line[:tmp.start(1)] + \
                       "(%s.at_ref %s).data" % (tmp.group(1), tmp.group(2)) + \
                       line[tmp.end(2) + 1:]
            result.append(line)
        self.content = "\n".join(result)
        return success

    def all_array_indexes(self):
        while self.array_indexes():
            ...

    def scan_link_to(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(rf"(\s*)link\s+({WORD})\s+to\s+({WORD})\s*", line)
            if tmp is None:
                result.append(line)
            else:
                result.append(tmp.group(1) + tmp.group(3) + "." + word_last( tmp.group(2)) + " = " + tmp.group(2))
        self.content = "\n".join(result)

    def scan_extern(self):
        result = list()
        indent = -1
        externals = list()
        succeeded = False
        pub_all = False
        for line in self.content.split("\n"):
            if indent != -1:
                cur_indent = len(firsts(line))
                if cur_indent < indent:
                    indent = -1
                    externals.clear()
                    pub_all = False
                elif pub_all or cur_indent == indent:
                    for name in externals:
                        line = re.sub(r"([^\w.&])%s(\b)" % name, r"\1&%s\2" % name, line)
                result.append(line)
            else:
                tmp = re.match(r"(\s*)pub(_all)?\s+(.*)", line)
                if tmp is None:
                    result.append(line)
                else:
                    indent = tmp.group(1)
                    if tmp.group(2):
                        pub_all = True
                    for name in tmp.group(3).split(" "):
                        if len(name) > 0 and not name.isspace():
                            externals.append(name)
                    indent = len(indent)
                    succeeded = True
        self.content = "\n".join(result)
        return succeeded

    def scan_all_extern(self):
        self.content = re.sub(rf" ", r"\1pub \2\1\2 = \3", self.content)
        while self.scan_extern():
            ...

    def work(self, output_name: str) -> str:
        self.scan_returns()
        self.break_shortcuts()
        self.unmap_import()
        self.scan_import()
        self.use_statements()
        self.special_functions()
        self.all_array_indexes()
        self.scan_link_to()
        self.scan_all_extern()
        output = output_name + ".syn"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
