## 2024-05-18 - [CRITICAL] Prevent Command Injection and Path Traversal in Telemetry Module
**Vulnerability:** Command injection and path traversal due to unsanitized `filename` input in `TlmStartMonitoring` being used in `snprintf` to generate a shell script and shell command executed via `system()`.
**Learning:** Hardcoded calls to `system(buf)` where `buf` relies on external input can easily lead to arbitrary command execution. This pattern existed here to run gnuplot scripts.
**Prevention:** Avoid `system()` where possible. If inevitable, strongly sanitize any external input before constructing shell commands (e.g., allow only alphanumeric characters and underscores, discarding or replacing shell metacharacters and path delimiters).
