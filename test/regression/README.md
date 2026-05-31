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
| `TORCS_LIB` | `$TORCS_RUNTIME` | Module search path (`-L`) |
| `TORCS_DATA` | repo `data/` | Asset root (`-D`) |
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

## CMake runtime

If you built with CMake, point runtime at the build tree:

```bash
export TORCS_RUNTIME=/path/to/build/runtime
export TORCS_BIN=/path/to/build/bin/torcs-bin
./test/regression/run_regression.sh
```
