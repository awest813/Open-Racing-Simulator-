# SDL2 Migration Audit (Phase 0)

**Status:** SDL2 is the default Linux windowing path (CMake and autotools); use `--disable-sdl2` / `-DTORCS_USE_SDL2=OFF` for FreeGLUT.  
**Related:** [PHASE0.md](PHASE0.md), [PHASE1.md](PHASE1.md), [phase2.md](phase2.md)

## Summary

The current Linux/Windows interactive build uses **GLUT/FreeGLUT** for windowing and input (`src/linux/main.cpp`, `GfScrInit`). A search of `src/**/*.cpp` and `src/**/*.h` shows **no active SDL2 code paths** in the tree today; migration is greenfield on the platform layer, not “reviving” existing `ifdef SDL` blocks.

## Target outcome

- SDL2 owns window, GL context creation, events, and gamepad/keyboard input.
- Headless `torcs -r` remains GLUT-free (already true after `GfInit()` in console `main`).
- Graphics module (`ssggraph`) receives an existing GL context from SDL2 instead of GLUT.

## Work packages

| # | Task | Notes |
|---|------|-------|
| 1 | Add `find_package(SDL2)` to CMake (optional build flag) | `TORCS_USE_SDL2=ON` |
| 2 | Implement `src/linux/sdl2spec.cpp` mirroring `linuxspec.cpp` module loading | Keep `tGfOs` hooks |
| 3 | Replace `GfScrInit` / `glutMainLoop` path in `main.cpp` when SDL2 enabled | **Done (PoC):** `glutshim.h` + `sdl2glut.cpp` |
| 4 | Input abstraction in `tgfclient` | **Done (PoC):** SDL events → GLUT callbacks in `sdl2glut.cpp` |
| 5 | High-DPI, resizable window, multi-monitor | SDL2 `SDL_WINDOW_ALLOW_HIGHDPI` |
| 6 | Document dual build (GLUT legacy vs SDL2) until GLUT removal | Release notes |

## Risks

- PLIB SSG expects GL context lifetime compatible with SDL — test context recreation on resize.
- Split-screen and movie capture paths in `racegl.cpp` need explicit SDL validation.

## Build (Linux)

**Default (SDL2):**

```bash
sudo apt-get install libsdl2-dev   # Debian/Ubuntu
./configure
make
```

**Legacy FreeGLUT:**

```bash
./configure --disable-sdl2
make
```

**CMake (SDL2 on by default for Linux):**

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
# FreeGLUT: cmake -S . -B build -DTORCS_USE_SDL2=OFF
```

Headless `torcs -r` is unchanged (no GLUT/SDL).

Implementation files:

- `src/libs/tgfclient/glutshim.h` — includes FreeGLUT or `sdl2glut.h`
- `src/libs/tgfclient/sdl2glut.{h,cpp}` — minimal GLUT API on SDL2

## Non-goals (Phase 0)

- Vulkan or modern renderer (Phase 1+ in [FORZA_VISION.md](FORZA_VISION.md)).
- Removing GLUT before SDL2 parity is proven in CI.
- SDL2 build in default CI ([`cmake-linux.yml`](../../.github/workflows/cmake-linux.yml), [`sdl2-build.yml`](../../.github/workflows/sdl2-build.yml)).
