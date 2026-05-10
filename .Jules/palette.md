## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-04-25 - File Upload Input Accessibility
**Learning:** In legacy `.ihtml` template forms, unassociated text descriptors for file upload fields (`<input type="file">`) are not read properly by screen readers, which often just announce "File chosen" without context.
**Action:** When updating forms, always ensure text descriptors are wrapped in semantic `<label for="id">` tags and corresponding `id` attributes are added to the `<input>` elements.
## 2024-05-24 - Improve click target sizes in legacy tabular forms
**Learning:** In legacy tabular layouts where inputs and descriptions are in separate `<td>` cells, wrapping the text in a `<label>` connected via `for` dramatically increases the clickable area, compensating for the lack of modern layout structures and significantly improving usability for tiny inputs like checkboxes.
**Action:** Always wrap adjacent descriptive text in `<td>` cells with `<label>` tags linked to their corresponding inputs to cross cell boundaries and maximize clickable areas.
