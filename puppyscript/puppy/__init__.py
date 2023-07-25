import os

from .assignment import Assignment
from .brackets import Brackets
from .expr_optim import ExprOptim
from .extract import Extract
from .formatting import Formatting
from .ids import set_module_name
from .macro_rules import MacroRules
from .mid_process import MidProcess
from .operators import Operators
from .preprocess import Preprocess
from .structure import Structure
from .syntactic import Syntactic
from .type_format import TypeFormat


def script(file: str, output_name: str, keep_temp: bool):
    prev_msg = 0

    def show_progress(msg: str):
        nonlocal prev_msg

        def positive(x: int):
            if x < 0: return 0
            return x

        length = len(msg)
        print("\b" * prev_msg, msg, " " * positive(prev_msg - length), sep="", end="", flush=True)
        prev_msg = max(prev_msg, len(msg))

    def delete_temp():
        nonlocal tmp, file
        if not keep_temp:
            os.remove(tmp)
        tmp = file

    set_module_name("_" + os.path.split(output_name)[1].replace(".", "_") + "_")

    show_progress("Preprocessing...")
    tmp = file = Preprocess(file).work(output_name)
    show_progress("Extracting literals...")
    file = Extract(file).work(output_name)
    delete_temp()
    show_progress("Doing line mid-process...")
    file = MidProcess(file).work(output_name)
    delete_temp()
    show_progress("Processing macro rules...")
    file = MacroRules(file).work(output_name)
    delete_temp()
    show_progress("Processing operator expressions...")
    file = Operators(file).work(output_name)
    delete_temp()
    show_progress("Changing syntactic rules...")
    file = Syntactic(file).work(output_name)
    delete_temp()
    show_progress("Formatting types...")
    file = TypeFormat(file).work(output_name)
    delete_temp()
    show_progress("Processing structures...")
    file = Structure(file).work(output_name)
    delete_temp()
    show_progress("Processing assignment statements...")
    file = Assignment(file).work(output_name)
    delete_temp()
    show_progress("Opening up brackets...")
    file = Brackets(file).work(output_name, 0)
    delete_temp()
    show_progress("Optimizing expressions...")
    file = ExprOptim(file).work(output_name)
    delete_temp()
    show_progress("Formatting short structures...")
    file = Formatting(file).work(output_name)
    delete_temp()
    show_progress("Re-opening up brackets...")
    file = Brackets(file).work(output_name, 1)
    delete_temp()
    show_progress("Done!")
    print()
    with open(file, "r") as f:
        with open(output_name + ".pups", "w") as out:
            out.write(f.read())
    delete_temp()
