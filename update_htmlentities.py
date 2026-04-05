import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    new_content = content

    # Replace `htmlentities(...)` with `htmlentities(..., ENT_QUOTES, 'UTF-8')`
    # We need to be careful not to match ones that already have ENT_QUOTES

    # Simple strategy: just find `htmlentities(` and if it doesn't contain `ENT_QUOTES`, replace it.
    # However, doing a simple regex search for `htmlentities\([^,)]+\)` could work
    # We want to match `htmlentities(var)` and replace with `htmlentities(var, ENT_QUOTES, 'UTF-8')`

    new_content = re.sub(
        r"htmlentities\(([^,)]+)\)",
        r"htmlentities(\1, ENT_QUOTES, 'UTF-8')",
        new_content
    )

    if new_content != content:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Fixed {filepath}")

for root, dirs, files in os.walk('torcs_racing_board'):
    for file in files:
        if file.endswith('.php') or file.endswith('.inc'):
            process_file(os.path.join(root, file))
