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
        if f'id="{name}"' not in re.search(f'<input[^>]*name="{name}"[^>]*type="file"[^>]*>', content).group():
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
        # Note the careful multiline match

        # Let's use a simpler string matching instead to avoid regex complexity
        lines = content.split('\n')
        for i, line in enumerate(lines):
            if f'name="{name}"' in line and 'type="file"' in line:
                # The input is on line i. The descriptive text is likely in a <td> above.
                # Find the previous <td> that contains text (not just tags)
                for j in range(i - 1, max(0, i - 10), -1):
                    if '<td' in lines[j] and '</td>' not in lines[j]:
                        # The text is likely on lines[j+1]
                        if j + 1 < len(lines):
                            text_line = lines[j+1]
                            stripped = text_line.strip()
                            if stripped and not stripped.startswith('<'):
                                # It's just text!
                                lines[j+1] = text_line.replace(stripped, f'<label for="{name}">{stripped}</label>')
                                modified = True
                                break
                    elif '<td' in lines[j] and '</td>' in lines[j]:
                        # Text and td on same line?
                        stripped = re.sub(r'<[^>]*>', '', lines[j]).strip()
                        if stripped and not 'label' in lines[j]:
                             # Extract the text and replace it with label
                             lines[j] = lines[j].replace(stripped, f'<label for="{name}">{stripped}</label>')
                             modified = True
                             break

        content = '\n'.join(lines)

    if modified:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Fixed {filepath}")

for f in glob.glob('torcs_racing_board/templates/*.ihtml'):
    fix_file(f)
