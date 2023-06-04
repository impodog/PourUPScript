import re


class Command:
    src: str
    target: str

    def __init__(self, src: str, target: str):
        self.src = src
        self.target = target

    def sub(self, s) -> str | None:
        if isinstance(s, Command):
            s.target = self.sub(s.target)
        else:
            return re.sub(self.src, self.target, s)


class Preprocess:
    commands: list[Command]
    content: str

    def __init__(self, file: str):
        self.commands = list()
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def content_replace(self, old: str, new: str):
        while True:
            result = re.search(r"\b%s\b" % old, self.content)
            if result is None:
                break
            self.content = self.content[:result.start(0)] + new + self.content[result.end(0):]

    def fix_line_continue(self):
        stack = str()
        result = str()
        for line in self.content.split("\n"):
            tmp = line.rstrip()
            if tmp.endswith("\\"):
                stack += tmp[:-1]
            else:
                result += stack + line + "\n"
                stack = str()
        self.content = result

    def predefined(self):
        self.content_replace("true", "1")
        self.content_replace("false", "0")

    def scan_defines(self):
        results = re.findall(r"#\{define\s*(.+?)\s*=\s*(.+?)\s*}", self.content)
        for result in results:
            tmp = Command(result[0], result[1])
            for cmd in self.commands:
                cmd.sub(tmp)
            self.commands.append(tmp)
        for cmd in self.commands:
            self.content = cmd.sub(self.content)

    def remove_prep_lines(self):
        tmp = str()
        for line in self.content.split("\n"):
            if line.find("#") == -1:
                tmp += line + "\n"
        self.content = tmp

    def work(self, output_name: str) -> str:
        self.fix_line_continue()
        self.predefined()
        self.scan_defines()
        self.remove_prep_lines()
        output = output_name + ".prep"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
