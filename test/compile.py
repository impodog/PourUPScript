import os
import subprocess

for file in os.listdir("."):
    if os.path.isfile(file) and file.endswith(".puppy"):
        subprocess.run(("python", "../puppyscript/main.py", file))
