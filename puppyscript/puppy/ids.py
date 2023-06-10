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


def is_word(s: str) -> bool:
    return re.fullmatch(r"\w+", s) is not None


temp_id = "_R_TMP"
targeting = "targeting"
moveTo = "mov"
ret = "return"
