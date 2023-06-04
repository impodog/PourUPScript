import os, sys, argparse
import puppy


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", type=str)
    parser.add_argument("--output", "-o", type=str, default=None)
    parser.add_argument("--keep-temp", type=bool, default=False, nargs="?", const=True)
    args = parser.parse_args()
    file: str = os.path.abspath(args.file)
    output: str | None = args.output
    keep_temp = args.keep_temp
    if output is None:
        output = ".".join(file.split(".")[:-1])
    puppy.script(file, output, keep_temp)


if __name__ == "__main__":
    main()
