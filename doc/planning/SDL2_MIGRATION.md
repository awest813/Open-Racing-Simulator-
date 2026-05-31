# SDL2 Migration Audit (Phase 0)

**Status:** Planning — 2026-05-31  
**Related:** [PHASE0.md](PHASE0.md), [phase2.md](phase2.md)

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
| 3 | Replace `GfScrInit` / `glutMainLoop` path in `main.cpp` when SDL2 enabled | Feature flag |
| 4 | Input abstraction in `tgfclient` | Map SDL events to existing key callbacks |
| 5 | High-DPI, resizable window, multi-monitor | SDL2 `SDL_WINDOW_ALLOW_HIGHDPI` |
| 6 | Document dual build (GLUT legacy vs SDL2) until GLUT removal | Release notes |

## Risks

- PLIB SSG expects GL context lifetime compatible with SDL — test context recreation on resize.
- Split-screen and movie capture paths in `racegl.cpp` need explicit SDL validation.

## Non-goals (Phase 0)

- Vulkan or modern renderer (Phase 1+ in [FORZA_VISION.md](FORZA_VISION.md)).
- Removing GLUT before SDL2 parity is proven in CI.
