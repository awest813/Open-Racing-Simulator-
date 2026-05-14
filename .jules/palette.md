## 2024-05-15 - [Initial Template Accessibility]
**Learning:** This legacy application uses `.ihtml` template files where text next to input fields is not wrapped in `<label for="id">` tags, making form inputs poorly accessible to screen readers, especially without corresponding `id` tags on the inputs.
**Action:** Always verify unassociated text labels next to inputs are correctly semantic. Adding `<label for="[id]">` and an `id="[id]"` to the `input` tags without visual disruption significantly improves a11y.

## 2024-05-15 - [Clickable Labels for Table Inputs]
**Learning:** In legacy tabular form designs, radio buttons and checkboxes are often separated from their text into different `<td>` cells. This makes them hard to click and inaccessible.
**Action:** Always wrap adjacent descriptive text in a `<label for="[id]" style="display:block;cursor:pointer;">` to span the cell boundary, creating a large clickable hit area that significantly improves UX.
