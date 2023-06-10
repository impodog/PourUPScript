from .preprocess import Preprocess
from .extract import Extract
from .operators import Operators
from .brackets import Brackets
from .assignment import Assignment
from .structure import Structure
import os


def script(file: str, output_name: str, keep_temp: bool):
    def delete_temp():
        nonlocal tmp, file
        if not keep_temp:
            os.remove(tmp)
        tmp = file

    tmp = file = Preprocess(file).work(output_name)
    file = Extract(file).work(output_name)
    delete_temp()
    file = Operators(file).work(output_name)
    delete_temp()
    file = Structure(file).work(output_name)
    delete_temp()
    file = Assignment(file).work(output_name)
    delete_temp()
    file = Brackets(file).work(output_name)
    delete_temp()
    with open(file, "r") as f:
        with open(output_name + ".pups", "w") as out:
            out.write(f.read())
    delete_temp()
