# Phase 0 — Foundation Checklist

Active work stream for the [Forza-class vision](FORZA_VISION.md). Check items off in PRs that reference this file.

## Build & platform

- [x] CMake: core libraries, `simuv2`/`simuv3`, track module
- [x] CMake: all 12 robot drivers (`src/drivers/CMakeLists.txt`)
- [x] CMake: Linux `torcs-bin` executable target
- [x] CMake: runtime asset sync target (`torcs_runtime_assets`)
- [ ] CMake: `ssggraph` graphics module fully linked on Linux
- [ ] CMake: `cmake --install` rules for portable runtime layout
- [ ] SDL2: audit existing code paths; document migration plan
- [ ] SDL2: minimal window + input proof-of-concept
- [ ] Break `raceman.h` ↔ `simu.h` circular include

## Validation

- [x] Headless race config for regression (`test/regression/regression-race.xml`)
- [x] Regression runner script (`test/regression/run_regression.sh`)
- [ ] Record first official baseline (`test/regression/baselines/`)
- [ ] CI: Ubuntu CMake build workflow
- [ ] CI: run regression when `torcs`/`torcs-bin` available

## Design

- [x] Replay format RFC ([`REPLAY_FORMAT.md`](REPLAY_FORMAT.md))
- [ ] Replay recorder spike (inputs only, single car)
- [ ] Content compliance wave 1 (replace `155-DTM`, `acura-nsx`, `mc-larenf1`, `p406`)

## Vertical slice (bridge to Phase 1)

- [ ] AI-only 10-lap **forza** regression baseline committed
- [ ] Document `-L` / `-D` / `-l` paths for dev-tree runs in CONTRIBUTING

---

**How to run regression locally**

```bash
# After ./configure && make && make install (or CMake build with torcs-bin on PATH)
./test/regression/run_regression.sh

# With explicit binary and runtime root
TORCS_BIN=/path/to/torcs-bin TORCS_RUNTIME=/path/to/runtime ./test/regression/run_regression.sh
```
