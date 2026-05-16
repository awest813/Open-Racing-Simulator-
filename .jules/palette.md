## 2024-05-15 - [Initial Template Accessibility]
**Learning:** This legacy application uses `.ihtml` template files where text next to input fields is not wrapped in `<label for="id">` tags, making form inputs poorly accessible to screen readers, especially without corresponding `id` tags on the inputs.
**Action:** Always verify unassociated text labels next to inputs are correctly semantic. Adding `<label for="[id]">` and an `id="[id]"` to the `input` tags without visual disruption significantly improves a11y.

## 2024-05-16 - [Radio Buttons in Tabular Layouts]
**Learning:** Legacy tabular form designs often separate radio buttons and their corresponding text into different `<td>` tags, making them functionally disconnected and inaccessible to screen readers.
**Action:** Adding `<label for="[id]" style="display:block;cursor:pointer;">` to wrap the unassociated text in the adjacent `<td>` cell drastically improves both accessibility and usability by providing a much larger click target, without altering the table structure.
