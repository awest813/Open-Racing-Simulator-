# Phase 2 Planning — Enabling the Future

Following the successful stabilization of the core 1.3.x line, Phase 2 focuses on high-impact architectural improvements and modernization of the platform layer. This phase prepares the codebase for version 1.4.

## 1. Build System Completion (CMake)

The transition to CMake has successfully stabilized the core libraries and simulation modules. The next step is full coverage:

- [ ] **Main Executable**: Transition `wtorcs` (Windows) and `torcs` (Linux) entry points to CMake.
- [ ] **Graphics Module**: Migrate `ssggraph` and resolve PLIB/OpenGL linking via CMake.
- [ ] **Driver Ecosystem**: Add all 12 robot drivers to the CMake target list.
- [ ] **Asset Deployment**: Implement `cmake install` rules for data assets (`cars`, `tracks`, `sounds`) to ensure a portable runtime environment.

## 2. Platform Layer Modernization (SDL2 Transition)

The current engine relies on legacy GLUT/FreeGLUT, which limits window management and input flexibility.

- [ ] **Revive SDL2 Path**: Audit the existing `ifdef SDL` blocks and bring them up to date with SDL 2.0+ conventions.
- [ ] **Input Abstraction**: Decouple joystick and keyboard handling from GLUT-specific callbacks.
- [ ] **High-DPI & Multi-Monitor**: Enable modern windowing features supported by SDL2.

## 3. Engineering Debt & Code Quality

- [ ] **Narrowing Conversion Pass**: Systematically address the remaining `C4244` and `C4267` warnings in physics calculations.
- [ ] **Structured Logging Expansion**: Roll out `GfOut` to the graphics and driver modules to unify diagnostic output.
- [ ] **Header Hygiene**: Address circular dependencies, particularly the `raceman.h <-> simu.h` knot.
- [ ] **Smart Pointers**: Begin targeted pilot implementations of `std::unique_ptr` for module lifetime management.

## 4. Content Compliance & Replacement

- [ ] **Asset Replacement (Phase 1)**: Replace the most legally sensitive car models (`155-DTM`, `acura-nsx`) with generic, high-quality equivalents.
- [ ] **Metadata Audit**: Ensure every `.png`, `.ac`, and `.xml` file has associated licensing metadata or falls under a clear global project license.

## 5. Automated Validation

- [ ] **Headless Regression Suite**: Create a scriptable test runner that executes a defined sequence of 10-lap races in headless mode and compares telemetry outputs (CSV) to a known-stable baseline.
- [ ] **Continuous Integration (Planning)**: Evaluate GitHub Actions or similar for automated CMake builds on both Linux and Windows.

---

Generated as part of the Phase 2 initialization — 2026-03-31
