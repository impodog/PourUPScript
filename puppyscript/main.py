import os, sys, argparse
import puppy


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", type=str)
    parser.add_argument("--output", "-o", type=str, default=None)
    parser.add_argument("--keep-temp", type=bool, default=False, nargs="?", const=True)
    parser.add_argument("--debug", type=bool, default=False, nargs="?", const=True)
    args = parser.parse_args()
    file: str = os.path.abspath(args.file)
    output: str | None = args.output
    keep_temp = args.keep_temp
    debug_mode = args.debug

    if not os.path.exists(file):
        raise FileNotFoundError("File not found: %s" % file)
    if not file.endswith(".puppy"):
        file += ".puppy"

    if output is None:
        output = ".".join(file.split(".")[:-1])
    else:
        if output.endswith(".pups"):
            output = output[:-5]
    puppy.script(file, output, keep_temp, debug_mode)


if __name__ == "__main__":
    main()
