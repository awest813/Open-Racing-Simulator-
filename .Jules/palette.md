## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-05-19 - Accessible Labels for Fallback Error Checkboxes
**Learning:** In legacy `.ihtml` forms, secondary or fallback inputs (like "I could not run the race" checkboxes) frequently lack semantic `<label>` tags linking them to adjacent descriptive text. Because checkboxes are visually small, this lack of structure makes them very difficult to target, both for pointing devices and assistive technologies.
**Action:** Always wrap adjacent descriptive text in `<label for="checkbox_id">` tags and apply a matching `id="checkbox_id"` to the corresponding `input` element. This adheres to core HTML accessibility standards without affecting presentation and drastically improves the functional tap target size.
