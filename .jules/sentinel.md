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
## 2024-05-24 - Hardcoded database credentials
**Vulnerability:** The legacy `torcs_racing_board` PHP application hardcoded its database credentials directly into `secrets/configuration.php`.
**Learning:** Hardcoded credentials in source code files are a critical security vulnerability, especially if the repository becomes public.
**Prevention:** Use environment variables (e.g., `getenv()`) to retrieve sensitive configuration like database credentials, and provide safe fallbacks if necessary to maintain backward compatibility without storing actual secrets.
