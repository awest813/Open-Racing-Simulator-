## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.

## 2024-05-19 - Associating Semantic Labels to File Inputs in Legacy Forms
**Learning:** Legacy template forms often lack semantic `<label>` associations for file inputs (e.g. `type="file"`). This causes screen readers to announce the input as just "File chosen" or "No file chosen" without context of what the file upload is actually for (e.g. "Team Logo").
**Action:** Always wrap adjacent descriptive text in a `<label for="input_id">` tag and apply the corresponding `id` to the file input to ensure full keyboard and screen reader accessibility for file uploads in legacy environments.
