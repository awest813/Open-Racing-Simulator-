## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-05-18 - Ensuring Labels Wrap Inputs for Checkboxes and File Inputs
**Learning:** In legacy `.ihtml` templates, descriptive text next to input elements like `type="checkbox"` or `type="file"` is often isolated in its own `<td>` or `<p>`, completely disassociated from the input.
**Action:** Adding explicit `id` attributes to the input elements and wrapping the isolated descriptive text in matching `<label for="id">` tags instantly improves usability, allowing users to click the text to interact with the input element, without needing to redesign the HTML structure.
