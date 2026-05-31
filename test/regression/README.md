# Headless regression races

Scriptable checks for physics, robot, and race-manager changes. Part of [Phase 0](../../doc/planning/PHASE0.md).

## Prerequisites

- Built and installed simulator (`torcs` or `torcs-bin` on `PATH`), **or** set `TORCS_BIN`
- Robot modules available under the runtime `drivers/` directory
- **No human drivers** in the race XML (`torcs -r` rejects human modules)

## Quick run

From the repository root:

```bash
./test/regression/run_regression.sh
```

Environment overrides:

| Variable | Default | Purpose |
|----------|---------|---------|
| `TORCS_BIN` | `torcs` then `torcs-bin` | Simulator executable |
| `TORCS_RUNTIME` | repo `runtime/` | Local config root (`-l`) |
| `TORCS_LIB` | repo `export/` | Module and driver `.so` path (`-L`) |
| `TORCS_DATA` | repo root | Install root (`-D`); engine loads `data/cars/...` below this |
| `REGRESSION_CONFIG` | `test/regression/regression-race.xml` | Race manager XML |
| `REGRESSION_BASELINE` | `test/regression/baselines/regression-race.winner` | Expected P1 driver module |

## Baselines

After a known-good build, record the race winner (first place module name):

```bash
./test/regression/record_baseline.sh
```

Commit `test/regression/baselines/regression-race.winner` when the simulation outcome is intentionally updated.

## Race definition

[`regression-race.xml`](regression-race.xml): **forza** track, **10 laps**, four AI robots (`berniw`, `olethros`, `inferno`, `bt`) with fixed car indices.

Results are written under `$TORCS_RUNTIME/results/regression-race/` (filename includes timestamp).

## Autotools dev tree

After `./configure && make`, modules land in `export/` and config in `runtime/`:

```bash
export TORCS_BIN="$PWD/src/linux/torcs-bin"
export TORCS_LIB="$PWD/export"
export TORCS_DATA="$PWD"
export TORCS_RUNTIME="$PWD/runtime"
./test/regression/run_regression.sh
```

## CMake runtime

If you built with CMake, point runtime at the build tree:

```bash
export TORCS_RUNTIME=/path/to/build/runtime
export TORCS_BIN=/path/to/build/bin/torcs-bin
export TORCS_LIB=/path/to/build/runtime
export TORCS_DATA=/path/to/build/runtime
./test/regression/run_regression.sh
```
