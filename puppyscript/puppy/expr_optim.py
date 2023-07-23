class ExprOptim:
    content: str

    def __init__(self, file: str):
        with open(file, "r", encoding="utf-8") as f:
            self.content = f.read()

    def work(self, output_name: str) -> str:
        output = output_name + ".expr_opt"
        with open(output, "w", encoding="utf-8") as f:
            f.write(self.content)
        return output
