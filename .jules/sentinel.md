## 2024-05-18 - [CRITICAL] Prevent Command Injection and Path Traversal in Telemetry Module
**Vulnerability:** Command injection and path traversal due to unsanitized `filename` input in `TlmStartMonitoring` being used in `snprintf` to generate a shell script and shell command executed via `system()`.
**Learning:** Hardcoded calls to `system(buf)` where `buf` relies on external input can easily lead to arbitrary command execution. This pattern existed here to run gnuplot scripts.
**Prevention:** Avoid `system()` where possible. If inevitable, strongly sanitize any external input before constructing shell commands (e.g., allow only alphanumeric characters and underscores, discarding or replacing shell metacharacters and path delimiters).
## 2024-11-06 - Insecure Deserialization in Session Cookie
**Vulnerability:** The legacy TORCS PHP application used `unserialize()` directly on user-provided cookies in the `_checkRemembered` function to restore session variables.
**Learning:** This is a classic insecure deserialization vulnerability that can lead to Remote Code Execution (RCE) via PHP object injection. The original code was written for legacy PHP versions where security standards were different.
**Prevention:** Always treat cookie data as untrusted. Use `json_encode()` and `json_decode()` instead of `serialize()` and `unserialize()` when exchanging structured data via user-controllable input (like cookies). This prevents instantiation of arbitrary PHP objects during decoding.
