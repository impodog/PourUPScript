import re

pattern = r"\b(-?\d+(?:\.\d+)?)\b"
content = "The temperature is -3.5 degrees Celsius."

match = re.search(pattern, content)
if match:
    number = match.group(1)
    print("Found:", number)
else:
    print("Not found.")
