# Open Racing Simulator Modernization Report

## 1. Structured Logging Implementation (`simuv2` & `simuv3`)

Replaced raw C-style `printf` and `fprintf` diagnostic calls with the structured logging macros (`GfOut`, `GfError`, etc.) across the simulation modules to ensure proper debug integration. 

**Files Refactored:**
- `src/modules/simu/simuv2/car.cpp`: Replaced `printf` arrays in `SimTelemetryOut` with `GfOut`.
- `src/modules/simu/simuv3/car.cpp`: Replaced `printf` in telemetry and wheel data logs with `GfOut`.
- `src/modules/simu/simuv3/susp.cpp`: Modified suspension warnings from `fprintf(stderr, ...)` to `GfError(...)`.
- `src/modules/simu/simuv3/aero.cpp`: Replaced theoretical lift coefficient `fprintf` error bounds and warnings with `GfError` and `GfOut`.

*(Note: Instances protected by `#if 0` macro statements or localized to commented-out `//printf` code blocks were left as-is to preserve historical logic while ensuring active code complies with the modernization).*

## 2. Legacy C-String API Audit (`strcpy` modernization)

An extensive audit was performed across the `src/libs` and `src/drivers` namespaces aiming to replace legacy, bounds-unsafe string mutations (`strcpy`, `sprintf`) with `strncpy`/`strlcpy` equivalents.

**Findings:**
A deep recursive grep of the `src/libs` and `src/drivers` environments yielded no surviving implementations of `strcpy` or `sprintf` operations handling application logic. The only existing reference to `strcpy` within the entire source tree is isolated to the third-party PLIB dependency header file (`src/windows/include/plib/ul.h`), which is external to our proprietary codebase modernization scope.

It is highly likely that previous modernization sweeps successfully purged legacy string mutation from the target logic. Bound-safe `snprintf` variants are currently enforced where dynamical string allocation occurs (e.g., `simu.cpp`, `engine.cpp`). Other instances of C string handling, such as `strcmp`, are employed for read-only comparisons and do not trigger buffer overflow vulnerabilities.

## 3. Next Actions for Codebase Stabilization
1. **Build Environment Regeneration**: Persistent "file not found" and unresolved dependencies regarding core headers like `tgf.h` and primitive types (`tdble`) point to issues with MSBuild and configuration definitions. Replacing `.dsp`/`.vcproj` generation with a robust `CMakeLists.txt` is an absolute necessity to compile and validate the decoupled physics and structured logging changes.
2. **Deterministic Regression Tests**: Once the compiler environment is cleanly established, assert the physics decoupling (calculated skid parameters in smoke/fx modules) through isolated unit testing.

## 4. Race Initialization String-Safety Pass

The race-engine client still had a startup-time cluster of repeated `strncpy(..., MAX_NAME_LEN - 1)` patterns in `src/libs/raceengineclient/raceinit.cpp` when populating driver, team, car, module, and category metadata.

**Changes made:**
- Added a local `copyCString()` helper that accepts fixed-size character arrays, tolerates `nullptr`, truncates safely, and always writes a trailing null terminator.
- Replaced the repeated manual `strncpy` + sentinel writes for `_modName`, `_name`, `_teamname`, `_carName`, and `_category` with the helper.
- Verified the change by rebuilding the `raceengineclient` target successfully from the existing `build_x86` tree.

**Why this matters:**
This keeps a frequently exercised initialization path behaviorally identical while reducing copy/paste string handling and eliminating the risk of future max-length edits forgetting the explicit null terminator.

## 5. GUI / Telemetry / Renderer String-Safety Pass

The remaining active `strncpy` call sites were clustered in GUI text widgets, telemetry filename sanitizing, and the OpenGL renderer's track-path handling.

**Changes made:**
- Added a shared `gfuiCopyLabelText()` helper for GUI labels and reused it in label, button, and edit-box setup/update paths.
- Replaced edit-box cursor scratch buffers with length-aware `std::string` prefix calculations, removing a fixed-size intermediate buffer from cursor placement logic.
- Replaced the telemetry filename copy and the OpenGL renderer track-directory copy with bounded `snprintf()`-based copies that preserve explicit truncation and null termination.

**Why this matters:**
This consolidates text-buffer handling in the GUI code, removes a hidden overflow risk in edit-box cursor math, and keeps the remaining non-GUI string copies consistent with the broader modernization direction.

## 6. MSVC GUI Warning Cleanup

Two pre-existing MSVC deprecation warnings remained in the GUI library: `localtime` in screenshot naming and `sscanf` in screen-configuration persistence.

**Changes made:**
- Added a small cross-platform `gfuiGetLocalTime()` helper in `src/libs/tgfclient/gui.cpp` that uses `localtime_s` on Windows and safe fallbacks elsewhere.
- Replaced `sscanf`-based parsing in `src/libs/tgfclient/screen.cpp` with explicit `strtol`-based integer and resolution parsers.

**Why this matters:**
This removes the warning noise from the `tgfclient` build without relying on blanket warning suppression and makes the parsing/error paths more explicit.

## 7. Game-Mode Parser Cleanup

The remaining `sscanf` warning sites were in `src/libs/tgfclient/fg_gm.cpp`, inside the non-Windows GLUT game-mode string parser.

**Changes made:**
- Replaced the nested `sscanf` cascade with an explicit `strtol`-based parser that accepts the same optional game-mode components (`widthxheight`, `:depth`, `@refresh`) in the same combinations.
- Kept the existing fallback behavior: invalid strings still log an error and leave the default mode values in place.

**Why this matters:**
This removes the last `sscanf` usage from `src/libs/tgfclient` while making the accepted grammar easier to understand and maintain.

## 8. Parameter-Conversion Type-Safety Pass

The next modernization slice focused on parameter-reading code that still relied on C-style numeric casts in `src/libs/tgfclient` and the legacy track loaders.

**Changes made:**
- Replaced parameter-read casts with explicit `static_cast` conversions in `src/libs/tgfclient/glfeatures.cpp`, `src/modules/track/track.cpp`, `src/modules/track/track3.cpp`, and `src/modules/track/track4.cpp`.
- Kept the legacy `DoVfactor` behavior in `track4.cpp`, but documented that it is intentionally quantized through an integer conversion before being stored in a floating-point field.

**Why this matters:**
These changes do not alter runtime behavior, but they make narrowing conversions explicit, easier to review, and less likely to hide accidental truncation in future edits.
