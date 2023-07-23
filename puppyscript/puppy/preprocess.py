import re, os

from .ids import is_word, firsts, rules, default_rules


class Command:
    src: str
    target: str
    is_word: bool

    def __init__(self, src: str, target: str):
        self.is_word = is_word(src)
        if self.is_word:
            self.src = r"#*(\b)(%s)(\b)#*" % src
        else:
            self.src = src
        self.target = target

    def sub(self, s) -> str | None:
        if isinstance(s, Command):
            s.target = self.sub(s.target)
        else:
            if self.is_word:
                while True:
                    result = re.search(self.src, s)
                    if result is None:
                        return s
                    if result.group(1) == "#":
                        tmp = s[:result.start(0)] + self.target
                    else:
                        tmp = s[:result.start(2)] + self.target
                    if result.group(3) == "#":
                        tmp += s[result.end(0):]
                    else:
                        tmp += s[result.end(2):]
                    s = tmp
            else:
                return re.sub(self.src, self.target, s)


class Preprocess:
    commands: dict[str, Command]
    content: str

    def __init__(self, file: str):
        self.commands = dict()
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def content_replace(self, old: str, new: str):
        while True:
            result = re.search(r"\b%s\b" % old, self.content)
            if result is None:
                break
            self.content = self.content[:result.start(0)] + new + self.content[result.end(0):]

    def scan_includes(self):
        while True:
            result = list()
            end_scanning = True
            for line in self.content.split("\n"):
                tmp = re.fullmatch(r"\s*#include\s+(.+)", line)
                if tmp is None:
                    result.append(line)
                else:
                    end_scanning = False
                    file = tmp.group(1)
                    try:
                        with open(file, "r", encoding="utf-8") as f:
                            content = f.read()
                    except:
                        try:
                            with open("./include/" + file, "r", encoding="utf-8") as f:
                                content = f.read()
                        except:
                            with open(os.path.join(os.path.split(__file__)[0], "../../scripts/") + file, "r", encoding="utf-8") as f:
                                content = f.read()
                    result.append(content)
            self.content = "\n".join(result)
            if end_scanning:
                break

    def remove_comments(self):
        self.content = re.sub(r"/\*.*?\*/", "", self.content, flags=re.S)
        self.content = re.sub(r"//.+\n", "\n", self.content)

    def remove_empty_lines(self):
        result = list()
        for line in self.content.split("\n"):
            if len(line) > 0 and not line.isspace():
                result.append(line)
        result.append("")
        self.content = "\n".join(result)

    def fix_line_continue(self):
        stack = str()
        result = str()
        for line in self.content.split("\n"):
            tmp = line.rstrip()
            if tmp.endswith("\\"):
                stack += tmp[:-1] + " "
            else:
                result += stack + line + "\n"
                stack = str()
        self.content = result

    def predefined(self):
        ...

    def scan_setting(self):
        result = list()
        sets = set()
        env = True
        stack = list()
        for line in self.content.split("\n"):
            if line.startswith("#set") and env:
                tmp = re.fullmatch(r"#set\s*(.*)", line)
                if tmp is None:
                    raise RuntimeError("#set command got incorrect args")
                for name in tmp.group(1).split(" "):
                    if len(name) > 0 and not name.isspace():
                        sets.add(name)
            elif line.startswith("#unset") and env:
                tmp = re.fullmatch(r"#unset\s*(.*)", line)
                if tmp is None:
                    raise RuntimeError("#unset command got incorrect args")
                for name in tmp.group(1).split(" "):
                    if len(name) > 0 and not name.isspace():
                        sets.remove(name)
            elif line.startswith("#ifset"):
                if env:
                    tmp = re.fullmatch(r"#ifset\s*(.*)", line)
                    if tmp is None:
                        raise RuntimeError("#ifset command got incorrect args")
                    for name in tmp.group(1).split(" "):
                        if len(name) > 0 and not name.isspace():
                            stack.append(name in sets)
                            env = env and stack[-1]
                else:
                    stack.append(False)
            elif line.startswith("#ifnset"):
                if env:
                    tmp = re.fullmatch(r"#ifnset\s*(.*)", line)
                    if tmp is None:
                        raise RuntimeError("#ifnset command got incorrect args")
                    for name in tmp.group(1).split(" "):
                        if len(name) > 0 and not name.isspace():
                            stack.append(name not in sets)
                            env = env and stack[-1]
                else:
                    stack.append(False)
            elif line.startswith("#end"):
                stack.pop()
                env = all(stack)
            elif env:
                result.append(line)
        self.content = "\n".join(result)

    def scan_defines(self):
        result = list()
        for line in self.content.split("\n"):
            define = re.fullmatch(r"\s*#define\s+(.+?)\s*=\s*(.+)\s*", line)
            undef = re.fullmatch(r"\s*#undef\s+(.+?)\s*", line)
            if define is None:
                if undef is None:
                    for cmd in self.commands.values():
                        line = cmd.sub(line)
                    result.append(line)
                else:
                    for s in undef.group(1).split(" "):
                        if len(s) > 0 and not s.isspace():
                            try:
                                self.commands.pop(s)
                            except KeyError:
                                ...
            else:
                new_cmd = Command(define.group(1), define.group(2))
                for cmd in self.commands.values():
                    cmd.sub(new_cmd)
                self.commands[define.group(1)] = new_cmd
        self.content = "\n".join(result)

    def scan_rules(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(r"\s*#rule\s+(.+?)\s*=\s*(.+)\s*", line)
            if tmp is None:
                result.append(line)
            else:
                name = tmp.group(1)
                value = tmp.group(2)
                match value:
                    case "true":
                        value = True
                    case "false":
                        value = False
                    case "show":
                        print("Value for rule \"%s\" is %s." % (name, rules[name]))
                        return
                    case "default":
                        value = default_rules[name]
                    case _:
                        value = eval(value)
                if isinstance(value, (int, float, bool, str)):
                    rules[name] = value
                else:
                    raise RuntimeError("Rule got incorrect value : %s" % value)
        self.content = "\n".join(result)

    def remove_prep_lines(self):
        tmp = str()
        for line in self.content.split("\n"):
            if len(line) == 0 or line[0] != '#':
                tmp += line + "\n"
        self.content = tmp

    def work(self, output_name: str) -> str:
        self.scan_includes()
        self.remove_comments()
        self.remove_empty_lines()
        self.fix_line_continue()
        self.scan_setting()
        self.predefined()
        self.scan_defines()
        self.scan_rules()
        self.remove_prep_lines()
        output = output_name + ".prep"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
