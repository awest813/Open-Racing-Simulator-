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
## 2024-05-30 - Expand Clickable Targets with Labels in Tabular Layouts
**Learning:** In legacy tabular form layouts, adjacent images (like poll graphics) and text are often separated from their inputs across table cells. Wrapping the text and visual indicators together in a `<label for="...">` makes the entire graphical block clickable, drastically increasing the target size for users.
**Action:** Always include associated inline graphics and descriptive text inside `<label>` tags when improving UX for table-based inputs to maximize hit area without relying on custom CSS.

## 2024-06-03 - Use type="email" for Email Inputs in Legacy Templates
**Learning:** Legacy `.ihtml` template forms often use `<input type="text">` for email fields, which misses out on native browser validation and mobile keyboard optimization (showing the '@' symbol).
**Action:** Always update email input fields to use the semantic HTML5 `<input type="email">` attribute. This instantly improves mobile keyboard compatibility and enables native browser validation without requiring any custom CSS or additional dependencies.
## 2024-06-08 - HTML5 Client-Side Validation on Legacy Forms
**Learning:** Legacy forms often rely entirely on server-side validation, forcing a full page reload (and potentially losing non-persisted input) just to tell the user a required field was missed.
**Action:** Enhance legacy `.ihtml` forms by adding the HTML5 `required` attribute to mandatory `<input>` elements. This provides instant, native client-side validation and improves the UX without altering backend logic.
## 2024-06-17 - Unifying label targets across table cells
**Learning:** In legacy layouts, separating inputs (like checkboxes) and labels across different `<td>` elements disrupts click target unity. Expanding `colspan` and combining them into a single `<td>` inside one `<label>` significantly improves the target size.
**Action:** When updating tabular forms, merge the cells containing the input and text descriptor into a single cell, wrap both in a `<label>`.
