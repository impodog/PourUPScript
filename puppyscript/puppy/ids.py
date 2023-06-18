import re

_count = 1


def next_name(identifier: str) -> str:
    global _count
    count = _count
    identifier += "_"
    while count:
        identifier += chr(count % 26 + 97)
        count //= 26
    _count += 1
    return "_a" + identifier


def firsts(s: str, sub: str = "\t ") -> str:
    result = str()
    for c in s:
        if c not in sub:
            return result
        result += c
    return result


WORD = r"[\w.&]+"
INDENT = r"[^\S\n]*"


def is_word(s: str) -> bool:
    return re.fullmatch(rf"{WORD}", s) is not None


def stmt_add_brc(s: str) -> str:
    if is_word(s):
        return s
    return f"({s})"


def with_stmt_line(indent: str, var: str) -> str:
    if is_word(var):
        return indent + with_cmd + " " + var
    else:
        return indent + var


temp_id = "_R_TMP"
with_cmd = "with"
moveTo = "mov"
ret = "ret"
break_cmd = "break"
refTo = "ref"
del_cmd = "del"
