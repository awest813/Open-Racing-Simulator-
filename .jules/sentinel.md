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

## 2026-04-11 - Fix SQL Injection in Database Functions
**Vulnerability:** Several database utility functions (`existsTable`, `existsEntry`, `countHit`, `countSession`) were directly concatenating table names, column names, and values into SQL queries without sanitization, creating SQL injection vulnerabilities.
**Learning:** Legacy codebases often rely on string concatenation for SQL queries because they predate or don't use parameterized queries (like PDO or MySQLi). While parameterization is best, sometimes you must work within the constraints of the legacy system using existing escaping/quoting functions.
**Prevention:** Always sanitize or quote identifiers and values before inserting them into SQL queries, especially when refactoring to parameterized queries is not feasible. Ensure all inputs to database functions are treated as untrusted.
