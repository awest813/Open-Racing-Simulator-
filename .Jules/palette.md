## 2024-04-04 - Adding Proper Field Labels to Legacy Templates

**Learning:** Legacy `.ihtml` template structures often lack semantic `<label>` tags and matching `id` attributes on form elements, which significantly impairs screen reader support and keyboard navigation (e.g. clicking the label text to focus the input).

**Action:** When updating legacy template files, systematically verify that isolated descriptive text next to input fields is wrapped in a `<label for="field_id">` tag and ensure the corresponding `<input>`, `<select>`, or `<textarea>` has the matching `id="field_id"` attribute. This fixes micro-accessibility issues without changing visual styles.

## 2024-05-18 - Accessibility on Legacy .ihtml Forms
**Learning:** Legacy `.ihtml` template files in the `torcs_racing_board` project have many `input`/`select`/`textarea` elements without associated semantic labels, often presenting adjacent free text in `td` tags.
**Action:** Use standard `<label for="id">` tags around the free text, combined with explicit `id` attributes on the input fields, to improve structural and keyboard accessibility without requiring visual redesign or introducing new CSS frameworks.
## 2024-05-19 - Semantic labels in legacy .ihtml templates
**Learning:** Legacy PHP `.ihtml` templates in this codebase frequently feature unassociated text descriptions next to form inputs. These inputs lack explicit `id` attributes and their textual descriptions are not wrapped in `<label for="id">` tags, which makes them inaccessible to screen readers and difficult to click on for users.
**Action:** When updating or refactoring legacy templates like `forum_search.ihtml` or `forum_message_write.ihtml`, always ensure that descriptive text is wrapped in semantic `<label for="...">` tags and that the corresponding `id` is added to the `<input>` or `<select>` element. For inputs where visible text isn't desired due to design constraints, use a visually hidden label or `aria-label` where appropriate (e.g. `style="display:none;"` for `forum_search_input`).
