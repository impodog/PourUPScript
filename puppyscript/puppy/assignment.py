import re
from .ids import next_name, with_cmd, moveTo, with_stmt_line, stmt_add_brc


class Assignment:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_assign(self):
        result: list[str] = [""]
        for line in self.content.split("\n"):
            tmp = re.fullmatch(r"(\s*)([\w.]+)\s*=\s*(.+)", line)
            if tmp is None:
                result.append(line)
            else:
                prev = re.fullmatch(r"%s\s+([\w.]+)\s*" % (tmp.group(1) + moveTo), result[-1])
                if prev is not None and prev.group(1) == tmp.group(3):
                    result[-1] = tmp.group(1) + moveTo + " " + tmp.group(2)
                else:
                    result.append(with_stmt_line(tmp.group(1), tmp.group(3)))
                    result.append("%s %s" % (tmp.group(1) + moveTo, tmp.group(2)))
        self.content = "\n".join(result[1:])

    def work(self, output_name: str) -> str:
        self.scan_assign()
        output = output_name + ".no_assign"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
