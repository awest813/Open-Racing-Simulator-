# Career mode smoke tests

Headless checks and interactive flow notes for [careermenu.cpp](../../src/libs/raceengineclient/careermenu.cpp).

## Rookie round (headless)

`career-rookie.xml` includes a human driver, so `torcs -r` cannot run it. CI uses `career-rookie-headless.xml` (AI only, 3 laps on forza):

```bash
./test/career/run_rookie_race.sh

# CMake portable runtime
TORCS_BIN=build/bin/torcs-bin \
TORCS_RUNTIME=build/runtime \
TORCS_LIB=build/runtime \
TORCS_DATA="$(pwd)" \
./test/career/run_rookie_race.sh
```

Results are written to `results/<raceman-stem>/` (e.g. `career-rookie-headless/`). The career hub reads `results/career-rookie/` when you play the real `career-rookie.xml` from the menus.

## Rookie end-to-end (interactive)

1. Main menu → **Career**
2. **New Career** → enter name → **Start Career**
3. Career hub → **Race Next** (launches `config/raceman/career-rookie.xml`)
4. Complete the race; return to the hub to see standings and the next track

Season progress and promotion use the same results directory stem as the raceman file (`career-rookie`, `career-pro`, `career-elite`).
