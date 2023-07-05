import re

from .ids import WORD, firsts, next_name


class TypeFormat:
    content: str
    string_created: dict[str, str]

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()
        self.string_created = dict()

    def create_string(self, value: str):
        name = next_name("CLS")
        self.string_created[name] = value
        return name

    def add_variable_tags(self, body: str, indent: str) -> str:
        result = list()

        for line in body.split("\n"):
            cur_indent = firsts(line)
            if len(cur_indent) == len(indent):
                tmp = re.fullmatch(rf"(\s*(attr|mth))\s+(.+)\s*", line)
                if tmp is not None:
                    line = tmp.group(1)
                    for name in tmp.group(3).split(" "):
                        if len(name) > 0 and not name.isspace():
                            line += " " + self.create_string(name)
            result.append(line)
        return "\n".join(result)

    def add_method_tags(self, body: str, indent: str) -> str:
        result = list()
        is_begin_line = False
        for line in body.split("\n"):
            cur_indent = firsts(line)
            if is_begin_line:
                result.append(cur_indent + "param self")
                is_begin_line = False
            elif len(cur_indent) == len(indent):
                tmp = re.fullmatch(rf"(\s*)(func|proc)\s*({WORD})\s*:\s*", line)
                if tmp is not None:
                    result.append(cur_indent + "mth " + self.create_string(tmp.group(3)))
                    is_begin_line = True
            result.append(line)
        return "\n".join(result)

    def review_static(self, body: str, indent: str) -> str:
        result = list()
        for line in body.split("\n"):
            cur_indent = firsts(line)
            if len(cur_indent) == len(indent):
                tmp = re.fullmatch(r"(\s*)stat\s+(.+)", line)
                if tmp is not None:
                    line = tmp.group(1) + tmp.group(2)
            result.append(line)
        return "\n".join(result)

    def replace_class(self) -> bool:
        result = list()
        type_name = None
        inherit = None
        entering = False
        outer_indent = inner_indent = None
        class_body = list()
        succeeded = False

        def reset():
            nonlocal type_name, outer_indent, inner_indent, class_body, inherit
            type_name = None
            outer_indent = inner_indent = None
            class_body = list()
            inherit = None

        for line in self.content.split("\n"):
            cur_indent = firsts(line)
            if type_name is None:
                tmp = re.fullmatch(rf"(\s*)(enter\s+)?class\s+({WORD})(\s+inh\s+(.+))?\s*:\s*", line)
                if tmp is None:
                    result.append(line)
                else:
                    succeeded = True
                    outer_indent = tmp.group(1)
                    entering = tmp.group(2) is not None
                    type_name = tmp.group(3)
                    result.append(outer_indent + ("type " if tmp.group(2) is None else tmp.group(2)) + type_name + ":")
                    if tmp.group(4) is not None:
                        inherit_list = list()
                        inherit_names = tmp.group(5)
                        for name in inherit_names.split(" "):
                            if len(name) > 0 and not name.isspace():
                                inherit_list.append(name if name.startswith("&") else "&" + name)
                        inherit = " ".join(inherit_list)

            else:
                if inner_indent is None:
                    inner_indent = cur_indent
                    if not entering:
                        result.append(inner_indent + "tid " + self.create_string(type_name))
                    if inherit is not None:
                        result.append(inner_indent + "inh " + inherit)
                if len(cur_indent) < len(inner_indent):
                    body = "\n".join(class_body)
                    body = self.add_variable_tags(body, inner_indent)
                    body = self.add_method_tags(body, inner_indent)
                    body = self.review_static(body, inner_indent)
                    result.append(body)
                    result.append(line)
                    reset()
                else:
                    class_body.append(line)
        self.content = "\n".join(result)
        return succeeded

    def replace_all_classes(self):
        while self.replace_class():
            ...

    def work(self, output_name: str) -> str:
        self.replace_all_classes()
        output = output_name + ".type_form"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        with open(output_name + ".con", "a", encoding="utf-8") as f:
            for name, value in self.string_created.items():
                f.write("%s = %s\n" % (name, value))
        return output
