## 2024-03-20 - Reflected XSS via `$_SERVER` variables in PHP templates

**Vulnerability:**
Several places in the codebase passed `$_SERVER["PHP_SELF"]` and `$_SERVER["QUERY_STRING"]` directly into template variables without appropriate escaping, leading to Reflected Cross-Site Scripting (XSS). Even when `htmlentities()` was used, it lacked the `ENT_QUOTES` parameter, making it vulnerable to single-quote breakout if the value is placed in single-quoted HTML attributes.

**Learning:**
In PHP prior to version 8.1, the default behavior for `htmlentities()` and `htmlspecialchars()` uses `ENT_COMPAT`, which translates double quotes but leaves single quotes unescaped. When URLs containing user input are constructed for use in template hrefs, an attacker can use a single quote to break out of the HTML attribute and execute arbitrary JavaScript.

**Prevention:**
Always use `htmlspecialchars($string, ENT_QUOTES, "UTF-8")` when sanitizing URL components constructed from `$_SERVER` variables (like `PHP_SELF` and `QUERY_STRING`) before passing them to the template engine. Avoid relying on the default parameters of HTML encoding functions.
