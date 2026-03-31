## 2024-05-15 - [Initial Template Accessibility]
**Learning:** This legacy application uses `.ihtml` template files where text next to input fields is not wrapped in `<label for="id">` tags, making form inputs poorly accessible to screen readers, especially without corresponding `id` tags on the inputs.
**Action:** Always verify unassociated text labels next to inputs are correctly semantic. Adding `<label for="[id]">` and an `id="[id]"` to the `input` tags without visual disruption significantly improves a11y.
