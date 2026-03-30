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
