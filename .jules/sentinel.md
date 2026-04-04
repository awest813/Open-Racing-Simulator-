## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
