## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-05-24 - Accessibility improvements for legacy torcs_racing_board .ihtml template
**Learning:** In legacy applications that use `.ihtml` template files where direct rendering triggers database connection errors, testing via script injection wrapping the template content into full HTML (e.g., using Playwright's `page.set_content()`) effectively verifies accessibility elements like label `<for>` associations. Replacing floating unassociated text next to inputs (like `<p>Robot package:</p>`) with proper `<label>` elements provides critical context for screen readers.
**Action:** When working on similar legacy template files in `torcs_racing_board`, extract snippet text, test using `page.set_content()`, and prioritize converting purely decorative `<p>` tags accompanying form controls to semantic `<label>` tags with matching `for` and `id` attributes.
