import re
from .ids import next_name, with_cmd, moveTo, with_stmt_line, stmt_add_brc, is_word, refTo, del_cmd, WORD, INDENT


class Assignment:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def scan_assign(self):
        result: list[str] = [""]
        for line in self.content.split("\n"):
            tmp = re.fullmatch(rf"(\s*)(.+?)\s*=\s*(.+?)\s*", line)
            if tmp is None:
                result.append(line)
            else:
                prev = re.fullmatch(rf"%s\s+({WORD})\s*" % (tmp.group(1) + moveTo), result[-1])
                if prev is not None and prev.group(1) == tmp.group(3):
                    result[-1] = tmp.group(1) + moveTo + " " + tmp.group(2)
                else:
                    if is_word(tmp.group(2)):
                        result.append(with_stmt_line(tmp.group(1), tmp.group(3)))
                        result.append("%s %s" % (tmp.group(1) + moveTo, tmp.group(2)))
                    else:
                        new_name = next_name("ASL")
                        result.append("%s %s" % (tmp.group(1) + refTo, tmp.group(2)))
                        result.append("%s %s" % (tmp.group(1) + moveTo, new_name))
                        result.append(with_stmt_line(tmp.group(1), tmp.group(3)))
                        result.append("%s %s.data" % (tmp.group(1) + moveTo, new_name))
                        result.append("%s %s" % (tmp.group(1) + del_cmd, new_name))
        self.content = "\n".join(result[1:])

    def scan_import(self):
        result = list()
        for line in self.content.split("\n"):
            tmp = re.fullmatch(rf"(\s*)imp\s+({WORD})\s+as\s+({WORD})", line)
            if tmp is None:
                result.append(line)
            else:
                result.append("%s %s" % (tmp.group(1) + "imp", tmp.group(2)))
                result.append("%s %s" % (tmp.group(1) + moveTo, tmp.group(3)))
        self.content = "\n".join(result)

    def work(self, output_name: str) -> str:
        self.scan_assign()
        self.scan_import()
        output = output_name + ".no_assign"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
