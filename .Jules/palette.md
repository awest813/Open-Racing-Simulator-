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
## 2024-06-26 - Autocomplete Attributes on Login Forms
**Learning:** Missing `autocomplete` attributes on login fields prevent modern password managers from accurately autofilling credentials, which degrades the UX significantly for users relying on password managers.
**Action:** Always add `autocomplete="username"` and `autocomplete="current-password"` to standard login fields, alongside the `required` attribute to support native browser client-side validation.

## 2024-06-25 - Enhance Login Accessibility
**Learning:** Legacy `.ihtml` forms lacking `autocomplete` attributes block modern password managers from assisting users, leading to poor accessibility and UX.
**Action:** Add semantic `autocomplete` attributes (like `username` and `current-password`) and `required` attributes to login fields to enable native browser validation and password manager support without altering CSS.

## 2024-06-24 - Semantic Attributes for Legacy HTML Forms
**Learning:** Legacy raw HTML forms (.ihtml) often lack modern HTML5 semantic attributes that aid accessibility and usability.
**Action:** Added autocomplete="username" and autocomplete="current-password" along with the required attribute to login fields. This simple addition significantly improves the user experience by enabling native browser validation and password manager support without needing custom CSS or javascript.

## 2024-06-23 - Improve Login Form Accessibility with Autocomplete
**Learning:** Legacy `.ihtml` login forms often lack `autocomplete` attributes, which prevents password managers from easily interacting with them and reduces form completion efficiency.
**Action:** When updating login inputs, always add standard HTML5 `autocomplete="username"` and `autocomplete="current-password"` attributes, along with `required`, to seamlessly support modern password managers and add instant client-side validation.

## 2024-06-22 - Missing HTML5 Autocomplete in Legacy Authentication Forms
**Learning:** Legacy `.ihtml` authentication forms (e.g., login status bars) frequently omit `autocomplete` attributes, which prevents modern browsers and password managers from correctly classifying inputs and offering seamless credential autofill to users, degrading the overall UX.
**Action:** When working on login forms, ensure `autocomplete="username"` and `autocomplete="current-password"` (or `new-password` for registration) are explicitly added to the relevant `<input>` elements. This instantly restores password manager support and enhances accessibility without altering the visual design or relying on custom JavaScript.

