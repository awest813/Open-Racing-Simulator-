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
## 2026-04-25 - Reflected XSS in PC_CREATETEAMPAGE
**Vulnerability:** Unescaped output of `$_SERVER['PHP_SELF']` to the frontend template variable `PC_CREATETEAMPAGE` in `torcs_racing_board/teams/team_create.php`. This template variable is used in the form `action` attribute, allowing an attacker to break out of the quotes and inject arbitrary HTML or JavaScript via URL manipulation.
**Learning:** In legacy PHP applications that map raw server variables like `$_SERVER['PHP_SELF']` directly into template context via arrays without sanitization, XSS payloads can be injected effortlessly.
**Prevention:** Always wrap `$_SERVER['PHP_SELF']` using `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` prior to binding it to a template string that's rendered into HTML attributes.
## 2024-05-24 - Weak PRNG Vulnerability
**Vulnerability:** Use of `rand()` and `getrandmax()` for generating sensitive tokens like password resets and activation keys.
**Learning:** Legacy PHP applications often use weak PRNGs like `rand()` instead of stronger alternatives. This compromises security since the output can be predicted.
**Prevention:** Always use at least `mt_rand()` and `mt_getrandmax()` instead of `rand()` and `getrandmax()`, or ideally cryptographically secure functions like `random_int()` when generating security-sensitive tokens.
## 2024-05-24 - Weak PRNG Vulnerability
**Vulnerability:** Use of `mt_rand()` and `md5()` for generating sensitive tokens like password resets and activation keys.
**Learning:** Legacy PHP applications often use weak PRNGs like `mt_rand()` instead of stronger alternatives. This compromises security since the output can be predicted.
**Prevention:** Always use cryptographically secure functions like `random_bytes()` when generating security-sensitive tokens.
## 2024-05-25 - Reflected XSS in admin/news_create.php
**Vulnerability:** Legacy PHP assignments pass `$_SERVER['PHP_SELF']` directly to `.ihtml` template engine variables (e.g., `PC_CREATENEWSPAGE` and `PS_LOGINPAGE`) without sanitization, leaving form `action` attributes unescaped and vulnerable to Reflected XSS attacks via URL path manipulation.
**Learning:** `$_SERVER['PHP_SELF']` includes raw URL-encoded characters up to the question mark (`?`). If placed directly into a form's action attribute without encoding single and double quotes, an attacker can break out of the HTML attribute and inject arbitrary scripts.
**Prevention:** Always wrap variables generated from the URL or headers (like `$_SERVER['PHP_SELF']`) using `htmlspecialchars($var, ENT_QUOTES, 'UTF-8')` before injecting them into HTML context.
