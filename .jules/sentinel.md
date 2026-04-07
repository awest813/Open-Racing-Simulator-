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

## 2024-05-18 - Reflected XSS via Unsanitized `$_SERVER['PHP_SELF']`
**Vulnerability:** The PHP `$_SERVER['PHP_SELF']` variable is directly assigned to template variables (e.g., `PC_REGISTERPAGE`, `PC_LOSTPASSWORDPAGE`) and output into HTML forms without sanitization. This allows an attacker to append arbitrary HTML/JavaScript to the URL path (e.g., `/register.php/%22%3E%3Cscript%3Ealert(1)%3C/script%3E`), resulting in Reflected Cross-Site Scripting (XSS).
**Learning:** `$_SERVER['PHP_SELF']` contains user-controlled input (the path info) and should never be trusted. The `torcs_racing_board` codebase frequently uses it to generate self-referencing form actions, making it a widespread attack vector.
**Prevention:** Always sanitize `$_SERVER['PHP_SELF']` and any other user-controllable `$_SERVER` variables using `htmlspecialchars($_SERVER['PHP_SELF'], ENT_QUOTES, 'UTF-8')` before passing them to the view/template layer.
