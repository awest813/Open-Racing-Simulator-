## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
## 2024-04-05 - Reflected XSS in PHP_SELF
**Vulnerability:** Legacy PHP assignments pass `$_SERVER['PHP_SELF']` directly to `.ihtml` template engine variables (e.g., `PS_LOGINPAGE`) without sanitization, leaving form `action` attributes unescaped and vulnerable to Reflected XSS attacks via URL path manipulation.
**Learning:** `$_SERVER['PHP_SELF']` includes raw URL-encoded characters up to the question mark (`?`). If placed directly into a form's action attribute without encoding single and double quotes, an attacker can break out of the HTML attribute and inject arbitrary scripts.
**Prevention:** Always wrap variables generated from the URL or headers (like `$_SERVER['PHP_SELF']`, `$_SERVER['REQUEST_URI']`, or `$_SERVER['QUERY_STRING']`) using `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` before injecting them into HTML context.
## 2024-04-08 - Reflected XSS via `$_SERVER['PHP_SELF']`
**Vulnerability:** Reflected Cross-Site Scripting (XSS) occurs when the legacy application assigns `$_SERVER['PHP_SELF']` directly to template variables like `PS_LOGINPAGE` or `PC_REGISTERPAGE` without any output encoding. An attacker can craft malicious URLs containing payload inside the path info, which is then rendered unsanitized in the HTML template attributes (e.g., `action` or `href`).
**Learning:** Legacy PHP apps often lack automatic context-aware escaping in their custom template engines (like `.ihtml` with `$page->set_var()`), trusting server variables like `$_SERVER['PHP_SELF']` blindly.
**Prevention:** Always wrap dynamically generated URL components derived from user or browser requests (like `$_SERVER['PHP_SELF']`, `$_SERVER['QUERY_STRING']`) in `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` before passing them to the presentation layer.
