import os
import re

simu_dir = r"d:\Games\Open-Racing-Simulator-\src\modules\simu"

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='latin-1') as f:
            content = f.read()

        new_content = re.sub(r'\bprintf\b', 'GfOut', content)

        if new_content != content:
            with open(filepath, 'w', encoding='latin-1') as f:
                f.write(new_content)
            print(f"Updated {filepath}")
    except Exception as e:
        print(f"Error {filepath}: {e}")

for root, _, files in os.walk(simu_dir):
    for filename in files:
        if filename.endswith(".cpp") or filename.endswith(".h"):
            process_file(os.path.join(root, filename))
