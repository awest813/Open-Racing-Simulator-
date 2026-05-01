## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.
## 2024-05-18 - Improved clickability for tiny inputs in legacy tabular forms
**Learning:** In legacy tabular form layouts (using `<table>`, `<tr>`, `<td>`), descriptive text for inputs is often placed in an adjacent `<td>` cell. Because screen readers might not associate them properly, and users have to precisely click tiny inputs (like checkboxes or radio buttons), wrapping the descriptive text in a `<label for="...">` tag that matches the input's `id` drastically improves UX. It expands the clickable target across table cell boundaries and correctly ties the label to the input for assistive technologies.
**Action:** Always check legacy tabular forms to ensure inputs, particularly tiny ones like checkboxes and file uploads, have `<label>` tags with matching `for`/`id` attributes, even if the text is physically separated into another layout cell.
