import os
import re

def fix_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Find cases where the previous cell has text without a label for the input
    # E.g.
    # <td class="filledlist">
    #   Image (*.jpg):
    # </td>
    # <td class="filledlist">
    #   <input name="car_image" type="file" ...>

    # Let's check for inputs without ids
    inputs_without_id = re.findall(r'<input[^>]*name="([^"]+)"[^>]*type="file"[^>]*>', content)
    for name in inputs_without_id:
        if 'id="' not in re.search(f'<input[^>]*name="{name}"[^>]*type="file"[^>]*>', content).group():
            print(f"Adding ID to {name} in {filepath}")
            content = re.sub(f'(<input[^>]*name="{name}")', f'\\1 id="{name}"', content)

    # Now find the corresponding text cells and add labels
    for name in inputs_without_id:
        # We look for the row containing this input.
        # This is complex with regex, better to write a small parser or do specific replacements.
        pass

    with open(filepath, 'w') as f:
        f.write(content)

import glob
for f in glob.glob('torcs_racing_board/templates/*.ihtml'):
    fix_file(f)
