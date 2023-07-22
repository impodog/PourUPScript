from .ids import set_module_name
from .preprocess import Preprocess
from .extract import Extract
from .mid_process import MidProcess
from .operators import Operators
from .assignment import Assignment
from .type_format import TypeFormat
from .structure import Structure
from .brackets import Brackets
from .formatting import Formatting
import os


def script(file: str, output_name: str, keep_temp: bool):
    def delete_temp():
        nonlocal tmp, file
        if not keep_temp:
            os.remove(tmp)
        tmp = file

    set_module_name("_" + os.path.split(output_name)[1].replace(".", "_") + "_")

    tmp = file = Preprocess(file).work(output_name)
    file = Extract(file).work(output_name)
    delete_temp()
    file = MidProcess(file).work(output_name)
    delete_temp()
    file = Operators(file).work(output_name)
    delete_temp()
    file = TypeFormat(file).work(output_name)
    delete_temp()
    file = Structure(file).work(output_name)
    delete_temp()
    file = Assignment(file).work(output_name)
    delete_temp()
    file = Brackets(file).work(output_name, 0)
    delete_temp()
    file = Formatting(file).work(output_name)
    delete_temp()
    file = Brackets(file).work(output_name, 1)
    delete_temp()
    with open(file, "r") as f:
        with open(output_name + ".pups", "w") as out:
            out.write(f.read())
    delete_temp()
