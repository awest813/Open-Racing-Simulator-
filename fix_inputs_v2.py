import os
import re
import glob

def fix_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Find cases where the previous cell has text without a label for the input
    inputs_without_id = re.findall(r'<input[^>]*name="([^"]+)"[^>]*type="file"[^>]*>', content)

    modified = False

    for name in inputs_without_id:
        # 1. Add ID to input if missing
        if 'id="' not in re.search(f'<input[^>]*name="{name}"[^>]*type="file"[^>]*>', content).group():
            content = re.sub(f'(<input[^>]*name="{name}")', f'\\1 id="{name}"', content)
            modified = True

        # 2. Add <label> wrapping the descriptive text in the preceding table cell
        # We look for:
        # <td class="filledlist"[^>]*>
        #   Some Text:
        # </td>
        # <td class="filledlist"[^>]*>
        #   <input ... name="{name}"

        # Regex to match the preceding <td> content before the <td> containing the input
        pattern = r'(<td[^>]*>)\s*([^<]+?)\s*(</td>\s*<td[^>]*>\s*<input[^>]*name="' + name + '")'

        match = re.search(pattern, content)
        if match:
            text_to_wrap = match.group(2).strip()
            # Ensure it's not already a label or contains HTML
            if not text_to_wrap.startswith('<label'):
                replacement = f'\\1\n\t\t\t\t\t\t\t<label for="{name}">{text_to_wrap}</label>\n\t\t\t\t\t\t\\3'
                content = re.sub(pattern, replacement, content, count=1)
                modified = True

    if modified:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Fixed {filepath}")

for f in glob.glob('torcs_racing_board/templates/*.ihtml'):
    fix_file(f)
