## 2024-05-18 - Fixed PHP Object Injection vulnerability in User class
**Vulnerability:** The `User` class in `torcs_racing_board/lib/classes.php` used `unserialize()` directly on user-controlled input (`$_COOKIE[COOKIE_NAME]`). This exposes the application to PHP Object Injection vulnerabilities, where an attacker could provide a malicious serialized string to instantiate arbitrary objects and potentially execute code via destructors or wakeup methods.
**Learning:** Legacy PHP applications often use `serialize()` and `unserialize()` as a quick way to store complex data structures in cookies. This is an anti-pattern because cookies are completely user-controlled.
**Prevention:** Always use `json_encode()` and `json_decode()` instead of `serialize()` and `unserialize()` for untrusted data like cookies, API payloads, or user input.
## 2024-03-30 - Fix XSS Vulnerability in PHP_SELF and QUERY_STRING variables

**Vulnerability:** Reflected Cross-Site Scripting (XSS) vulnerability found where `$_SERVER['PHP_SELF']` and `$_SERVER['QUERY_STRING']` were directly concatenated and passed to frontend templates (like `PS_LOGINPAGE` and `PC_POLL_PAGE`) without sanitization. An attacker could inject malicious scripts through manipulated URLs.
**Learning:** Legacy PHP code often assumes `$_SERVER` variables are safe because they originate from the server. However, components like `PHP_SELF` and `QUERY_STRING` are heavily influenced by the user's request path and parameters, making them prime vectors for XSS if echoed directly into HTML attributes.
**Prevention:** Always sanitize data originating from the user or the request URL (including `$_SERVER` variables) using `htmlentities()` or `htmlspecialchars()` before outputting it to templates or rendering it as HTML.
