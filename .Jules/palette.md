## 2026-03-27 - Add labels to login and registration forms
**Learning:** Explicitly associating `<label>` tags with `<input>` elements using the `for` and `id` attributes is a critical accessibility improvement, especially for checkboxes like "Remember Me", as it increases the clickable area and allows screen readers to correctly identify the input.
**Action:** Always ensure that every input field in a form has an associated label, particularly in older web applications where this standard practice might have been overlooked.
## 2026-03-28 - Form Accessibility in Legacy PHP Templates
**Learning:** Legacy `.ihtml` template structures often have form text disconnected from the `<input>` tags. The `lostpassword_request.ihtml` file showed raw text inputs relying solely on layout (e.g., table cells) without `<label>` elements or native form constraints.
**Action:** When updating form fields in older templates, systematically convert associated text to explicit `<label for="...">` wrappers, add missing `id` properties to inputs, and leverage HTML5 types (e.g., `type="email"`) and attributes (e.g., `required`) for native browser validation and better screen reader support.
