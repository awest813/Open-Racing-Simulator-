## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-04-25 - File Upload Input Accessibility
**Learning:** In legacy `.ihtml` template forms, unassociated text descriptors for file upload fields (`<input type="file">`) are not read properly by screen readers, which often just announce "File chosen" without context.
**Action:** When updating forms, always ensure text descriptors are wrapped in semantic `<label for="id">` tags and corresponding `id` attributes are added to the `<input>` elements.
## 2024-05-16 - Add semantic labels to cross-cell form inputs
**Learning:** In legacy tabular layouts, descriptive text often sits in a `<td>` separate from the input. Without explicit `<label for="...">` linking, screen readers fail to associate the text, and clicking the text does not focus the input.
**Action:** Always wrap descriptive text in a `<label>` and explicitly link it via `id` to the corresponding input, particularly for small touch targets like checkboxes and file inputs.
## 2024-05-24 - Maximize Click Target Area in Legacy Tabular Forms
**Learning:** In legacy layouts using `<table>` for forms, radio inputs and their associated text/images are often separated into distinct `<td>` cells. Wrapping the contents of the adjacent `<td>` cell within a `<label for="...">` effectively links the entire text block and graphical elements (like poll bar images) to the radio button, drastically increasing the clickable area without altering the table structure or relying on custom CSS.
**Action:** When modernizing tabular forms, proactively wrap adjacent descriptive content in a `<label>` referencing the input's `id` to enhance interaction targets, particularly for tiny controls like checkboxes and radio buttons.
