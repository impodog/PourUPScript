import re
import warnings

from .ids import firsts, is_debug_info


def ellipsis_name_re(name: str):
    return name.strip(".").replace(".", r"\.")


class Form:
    args: list[str]
    body: list[str]

    def __init__(self, args: str, body: str):
        self.args = list()
        for arg in args.split(" "):
            if len(arg) > 0 and not arg.isspace():
                self.args.append(arg)
        base_indent  = len(firsts(body))
        self.body = list()
        for line in body.split("\n"):
            self.body.append(line[base_indent:])

    def repl_with(self, assign: dict[str, str], base_indent: str) -> list[str]:
        result = list()
        is_first = True
        for line in self.body:
            for name, value in assign.items():
                tmp = re.search(r"(\b|#)%s(\b|#)" % name, line)
                if tmp is not None:
                    start = tmp.end(1)
                    end = tmp.start(2)
                    if tmp.group(1) == "#":
                        start -= 1
                    if tmp.group(2) == "#":
                        end += 1
                    line = line[:start] + value + line[end:]

            if is_first:
                result.append(line)
            else:
                result.append(base_indent + line)
            is_first = False
        return result


class Macro:
    name: str
    forms: list[Form]

    def __init__(self, name: str, forms: list[Form]):
        self.name = name
        self.forms = forms.copy()

    def try_repl(self, args_str: str, base_indent: str) -> str:
        args = list()
        for arg in args_str.split(" "):
            if len(arg) > 0 and not arg.isspace():
                args.append(arg.strip())
        arg_len = len(args)
        for form in self.forms:
            assign: dict[str, str] = dict()
            form_len = len(form.args)
            if form_len > arg_len:
                continue
            if form_len == 0:
                if arg_len == 0:
                    ...
                else:
                    continue
            elif form.args[-1].endswith("..."):
                for i in range(form_len - 1):
                    assign[form.args[i]] = args[i]
                assign[ellipsis_name_re(form.args[-1])] = " ".join(args[form_len - 1:])
            elif form.args[0].endswith("..."):
                for i in range(1, form_len):
                    assign[form.args[i]] = args[arg_len - form_len + i]
                assign[ellipsis_name_re(form.args[0])] = " ".join(args[:arg_len - form_len + 1])
            elif arg_len == form_len:
                for i in range(arg_len):
                    assign[form.args[i]] = args[i]

            if len(assign) == form_len:
                return "\n".join(form.repl_with(assign, base_indent))
        raise ValueError


class MacroRules:
    content: str
    macros: list[Macro]
    success: bool

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()
        self.macros = list()

    def repl(self, part: str, base_indent: str) -> str:
        if len(part) == 0 or part.isspace():
            return ""
        for macro in self.macros:
            tmp = re.search("%s<" % macro.name, part)
            if tmp is None:
                continue
            index = tmp.end()
            depth = 1
            while depth:
                match part[index]:
                    case "<":
                        depth += 1
                    case ">":
                        depth -= 1
                index += 1
            args = self.repl(part[tmp.end():index - 1], "")
            try:
                part = part[:tmp.start()] + macro.try_repl(args, base_indent) + part[index:]
            except:
                continue
            self.success = True
        return part

    def scan_macro_defs(self):
        result = list()
        name: str | None = None
        macro_outer: str | None = None
        form_args: str | None = None
        form_outer: str | None = None
        form_body: list[str] | None = None
        forms: list[Form] = list()

        def test_next_macro():
            nonlocal result, name, macro_outer
            tmp = re.fullmatch(r"\s*macro\s+(\w+(!?))\s*:\s*", line)
            if tmp is None:
                result.append(line)
            else:
                name = tmp.group(1)
                if not name.endswith("!"):
                    warnings.warn(
                        UserWarning("Macro names should end with \"!\". Consider changing \"%s\" to \"%s\"." % (
                            name, name + "!"))
                    )
                macro_outer = cur_indent

        def add_form():
            nonlocal form_args, form_body
            forms.append(Form(form_args, "\n".join(form_body)))
            form_args = None

        def start_form():
            nonlocal form_outer, form_args, form_body
            tmp = re.fullmatch(r"\s*form(\s+(\w[\w\s.]*))?\s*:\s*", line)
            if tmp is None:
                if not is_debug_info(line):
                    warnings.warn(
                        UserWarning("Stray line in macro forms: \"%s\"." % line)
                    )
            else:
                form_outer = cur_indent
                if tmp.group(1) is None:
                    form_args = ""
                else:
                    form_args = tmp.group(1)
                form_body = list()

        for line in self.content.split("\n"):
            cur_indent = firsts(line)
            if name is None:
                test_next_macro()
            else:
                if form_args is None:
                    start_form()
                else:
                    if len(cur_indent) == len(form_outer):
                        add_form()
                        start_form()
                    elif len(cur_indent) <= len(macro_outer):
                        add_form()
                        self.macros.append(Macro(name, forms))
                        name = form_args = None
                        forms.clear()
                        test_next_macro()
                    elif not is_debug_info(line):
                        form_body.append(line)
        self.content = "\n".join(result)

    def replace_all(self):
        self.success = True
        while self.success:
            result = list()
            self.success = False
            for line in self.content.split("\n"):
                line = self.repl(line, firsts(line))
                result.append(line)
            self.content = "\n".join(result)

    def work(self, output_name: str) -> str:
        self.scan_macro_defs()
        self.replace_all()
        output = output_name + ".macros"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
