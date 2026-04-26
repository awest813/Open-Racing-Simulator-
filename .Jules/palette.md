## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-05-19 - Using Labels to Expand Click Targets on Table Layouts
**Learning:** Legacy tabular layouts often separate a small `input type="checkbox"` into one `<td>` and its descriptive text into an adjacent `<td>`. Because the text is unlinked, users must click precisely on the tiny 13x13px checkbox to toggle it, which is poor UX for all users and fails WCAG target size guidelines.
**Action:** Always wrap the descriptive text in the adjacent table cell in a `<label for="checkbox_id">` tag. This not only fixes semantic screen reader accessibility but fundamentally improves mouse/touch UX by turning the entire text block into a large, comfortable click target that toggles the checkbox across `<td>` boundaries.
