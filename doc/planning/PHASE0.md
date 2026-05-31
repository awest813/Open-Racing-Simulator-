# Phase 0 — Foundation Checklist

**Status:** Complete (2026-05-31). Next: [Phase 1](PHASE1.md).

Foundation work stream for the [Forza-class vision](FORZA_VISION.md). Check items off in PRs that reference this file.

## Build & platform

- [x] CMake: core libraries, `simuv2`/`simuv3`, track module
- [x] CMake: all 12 robot drivers (`src/drivers/CMakeLists.txt`)
- [x] CMake: Linux `torcs-bin` executable target
- [x] CMake: runtime asset sync target (`torcs_runtime_assets`)
- [x] CMake: `ssggraph` graphics module on Linux (MODULE + PLIB link)
- [x] CMake: `cmake --install` rules for portable runtime layout
- [x] SDL2: audit — see [`SDL2_MIGRATION.md`](SDL2_MIGRATION.md)
- [x] SDL2: minimal window + input proof-of-concept (`--enable-sdl2`, `sdl2glut.cpp`)
- [x] Break `raceman.h` ↔ `simu.h` include cycle (`simuitf.h` extracted)

## Validation

- [x] Headless race config for regression (`test/regression/regression-race.xml`)
- [x] Regression runner script (`test/regression/run_regression.sh`)
- [x] Record first official baseline (`test/regression/baselines/regression-race.winner`)
- [x] CI: Ubuntu autotools build + regression ([`.github/workflows/regression.yml`](../../.github/workflows/regression.yml))
- [x] CI: CMake Linux workflow ([`.github/workflows/cmake-linux.yml`](../../.github/workflows/cmake-linux.yml))
- [x] Track path fallback in `ReInitTrack` (`tracks/<cat>/` then `tracks/<name>/`)

## Design

- [x] Replay format RFC ([`REPLAY_FORMAT.md`](REPLAY_FORMAT.md))
- [x] Replay recorder spike (`replayrecorder.cpp`, `TORCS_REPLAY_RECORD=1`)
- [x] Content compliance wave 1 — see [`CONTENT_COMPLIANCE.md`](CONTENT_COMPLIANCE.md)

## Vertical slice (bridge to Phase 1)

- [x] AI-only 10-lap **forza** regression baseline committed (sparkle on forza)
- [x] Document `-L` / `-D` / `-l` paths for dev-tree runs in CONTRIBUTING

---

**How to run regression locally**

```bash
# After ./configure && make && make install (or CMake build with torcs-bin on PATH)
./test/regression/run_regression.sh

# With explicit binary and runtime root
TORCS_BIN=/path/to/torcs-bin TORCS_RUNTIME=/path/to/runtime ./test/regression/run_regression.sh
```
