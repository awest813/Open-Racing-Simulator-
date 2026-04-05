## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
## 2024-05-24 - Default htmlentities() doesn't escape single quotes
**Vulnerability:** Reflected XSS in `torcs_racing_board` URL variables.
**Learning:** `htmlentities()` without the `ENT_QUOTES` flag leaves single-quoted attributes vulnerable to XSS.
**Prevention:** Always use `htmlentities($var, ENT_QUOTES, 'UTF-8')` or `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` when outputting user-controlled input, including server variables like `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']`, into HTML attributes.
## 2024-05-24 - Reflected XSS in Template Variable Assignment
**Vulnerability:** Reflected XSS through `$_SERVER['PHP_SELF']` assigned directly to template variables like `PS_LOGINPAGE` and `PS_LOGOUTPAGE` in `torcs_racing_board/index.php`.
**Learning:** Legacy PHP applications often directly assign request variables (`$_SERVER['PHP_SELF']`, `$_SERVER['QUERY_STRING']`) to template engines without intermediate sanitization, assuming the framework will handle it (which it often doesn't).
**Prevention:** Always wrap variables containing user-controlled input, including server variables used in URLs, with `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` before passing them to the view/template layer.
