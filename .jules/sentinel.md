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
## 2025-04-10 - Reflected XSS Vulnerabilities

**Vulnerability:** Found unescaped output of `$_SERVER['PHP_SELF']` to the frontend using `.ihtml` template assignments in `torcs_racing_board/account.php` and `torcs_racing_board/register.php`. This can be exploited by an attacker for Reflected XSS (e.g. by appending malicious payloads directly in the URL).
**Learning:** Legacy PHP code uses `.ihtml` templates and `set_var` assignments but relies on raw `$_SERVER['PHP_SELF']` which includes the unescaped path and payload. The templates assign them to `PC_ACCOUNTPAGE` and `PC_REGISTERPAGE` which is then embedded inside `action=""` tags.
**Prevention:** Always wrap `$_SERVER['PHP_SELF']` with `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` before setting it in a frontend variable or template in older PHP stacks, to ensure proper escaping of special characters.
## 2025-05-18 - Additional Reflected XSS in PHP_SELF
**Vulnerability:** Found more unescaped output of `$_SERVER['PHP_SELF']` to the frontend using string concatenation directly in PHP variables (e.g. `$delete_href` and `$hide_href`) in `torcs_racing_board/lib/functions_forum.php`. This allows an attacker to inject XSS payloads via the URL path.
**Learning:** Reflected XSS from `$_SERVER['PHP_SELF']` doesn't just happen through template assignments, but also when generating URLs internally via string concatenation inside PHP variables and logic. Even if you checked `.ihtml` template assignments, manual `href` strings generated inside PHP logic are equally vulnerable.
**Prevention:** Always wrap `$_SERVER['PHP_SELF']` with `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` immediately before it is used to generate HTML strings or URLs, regardless of whether it's assigned to a template or used in inline string concatenation.
