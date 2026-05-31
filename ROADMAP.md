# Open Racing Simulator — Roadmap

This document turns the historical TORCS backlog into a concrete plan for the
Open Racing Simulator fork.  It communicates direction and current priorities;
it does not promise exact delivery dates.

For the **Forza-class product vision** (phased delivery toward a modern,
moddable motorsport game), see [`doc/planning/FORZA_VISION.md`](doc/planning/FORZA_VISION.md)
and the active Phase 0 checklist [`doc/planning/PHASE0.md`](doc/planning/PHASE0.md).

The project has two parallel responsibilities:

1. Keep the current **1.3.x** line usable for players, modders, and researchers.
2. Prepare a healthier foundation for the larger architectural changes needed in
   **1.4** and **2.0**.

---

## Guiding Priorities

These apply across every work stream:

- Protect headless / automation workflows used for AI and research.
- Improve contributor documentation before expanding the feature surface.
- Modernize carefully — do not break existing cars, tracks, or robots.
- Reduce legal and licensing risk in bundled content.
- Replace fragile legacy dependencies only when a clear migration path exists.

---

## Status at a Glance

| Track | Status |
| :--- | :--- |
| Documentation & hygiene | Complete |
| 1.3.x stabilization | Complete (Core) |
| Engineering debt | Active / ongoing |
| 1.4 preparation (Phase 2) | Active |
| Content & compliance | Planning |
| 2.0 Networking | Future |
| Long-term exploration | Future |

---

## Documentation, Stability, and Project Hygiene

These items move first because they make every later release easier.

- [x] Refresh contributor documentation and setup guidance
- [x] Overhaul README with build instructions, feature overview, and architecture summary
- [x] Update the robot tutorial and race-manager documentation
- [x] Document important repository subsystems and extension points
- [x] Tighten the fork identity where old TORCS naming is confusing
- [x] Audit bundled assets for licensing, redistribution, and trademark issues

---

## 1.3.x Stabilization Track

**Goal:** keep the existing simulator practical while avoiding unnecessary API breaks.

### Simulation and Gameplay

- [x] Finish car adjustments for tire wear and performance balance
- [x] Improve ranking when damaged or stranded cars stay classified incorrectly
- [x] Make brake balance adjustable during a session
- [x] Add curb sound support
- [x] Investigate dynamic track-surface effects
- [ ] Investigate wind and ambient-temperature effects

### Analysis and Race Operations

- [x] Add a telemetry / data recorder
- [x] Expose more telemetry channels, including speed and suspension data
- [x] Add timed race formats
- [x] Fix repeated-violation edge cases when entering or exiting pit lanes
- [x] Add a data-analysis workflow for lap comparison and filtering
- [ ] Prototype a replay pipeline
- [ ] Expand configurable rules and penalty handling (1.4 breaking)

### Tooling and UX

- [x] Update Visual Studio project files and transition to CMake (Active)
- [x] Expose hidden or difficult-to-find settings in the GUI
- [x] Add GUI support for event blacklisting

---

## 1.4 Preparation Track

**Goal:** introduce breaking changes only after the 1.3.x line is better
documented and more maintainable.

### Robot API

- [ ] Review optional callbacks: `askfeatures`, `callonce`, `grid`, `postprocess`
- [ ] Identify robots by stable names instead of integer slots where possible
- [ ] Remove the fixed 10-driver-per-module limit

### Graphics and Platform

- [ ] Replace GLUT-era assumptions with a more modern windowing layer
- [ ] Review and revive the SDL 2 path
- [ ] Rework skid marks, masking, shadows, and frame-rate-dependent rendering
- [ ] Move graphics-engine state into an explicit context structure
- [ ] Reduce dynamic allocation inside rendering hot paths
- [ ] Review macOS support
- [ ] Review MorphOS support

### Audio

- [ ] Update OpenAL usage and remove deprecated ALUT dependencies
- [ ] Replace WAV-only assumptions with OGG Vorbis where appropriate
- [ ] Fix split-screen multiplayer audio issues

### Physics and Simulation

- [ ] Review simuv3 changes for selective merge into simuv2
- [ ] Review and merge the ABS work if it still applies cleanly
- [ ] Review and merge 4WD-related work if it still applies cleanly
- [ ] Add brake and engine wear with cooling effects
- [ ] Add configurable track-wall properties
- [ ] Extend track definitions for crossings, split/join segments, and variable width
- [ ] Consider differential adjustment during a session

### Content and Race Flow

- [ ] Add support for better starting / race modes, including multi-class
- [ ] Add support vehicles such as pace cars or trucks for incident cleanup
- [ ] Deliver a replay feature robust enough for long-term support

---

## Content and Compliance Track

**Goal:** keep the project redistributable and easier to package.

- [ ] Replace or retire trademark-sensitive car content
- [ ] Rework buggy and baja-bug assets
- [ ] Replace rally-car content that cannot be safely shipped long-term
- [ ] Remove invalid geometry from bundled tracks
- [ ] Audit car, track, and UI assets for missing licensing metadata

Specific replacements from the historical backlog:

- [ ] Replace `155-DTM`
- [ ] Replace `acura-nsx`
- [ ] Replace `mc-larenf1`
- [ ] Replace `p406`

---

## Engineering Debt Track

**Goal:** reduce maintenance cost without destabilizing the simulation.

### Already done

- [x] Enable C++14 (`-std=c++14`) across the entire build
- [x] Replace `NULL` with `nullptr` throughout the source tree
- [x] Migrate C headers (`<stdio.h>` etc.) to C++ equivalents (`<cstdio>` etc.)
- [x] Remove `register` keyword usage
- [x] Add `override` where missing in learning, music-player, and sound modules
- [x] Replace `printf`-style diagnostics with a structured logging approach
- [x] Replace remaining fragile string handling (`strcpy`, `sprintf`) with safer alternatives
- [x] Consolidate track loader allocation and parsing helpers

### Remaining

- [x] Reduce C-style casts and narrow unsafe conversions (raceengineclient, tgf)
- [x] Implement CMake build system for core libraries and modules
- [ ] Remove the circular dependency between `raceman.h` and `simu.h`
- [x] Move reusable spline code into a shared math library (`src/libs/math/spline.h`)
- [ ] Eliminate `glGet*` usage in simulation-time paths
- [ ] Refactor `trackgen` to reduce duplicated left/right segment logic
- [ ] Review whether PLIB usage can be isolated or retired incrementally
- [ ] Remove sound-property calculation from `wheel.cpp`
- [ ] Convert internal force-unit handling consistently (lbs → lbf)
- [x] Add a broader automated test suite for parsing and utility code (spline + vector tests)

---

## 2.0 Networking Track

**Goal:** introduce multiplayer only after the simulation and APIs are easier to
evolve safely.

### Pre-release exploration

- [ ] Design the network architecture and simulation-loop integration points
- [ ] Build a networking proof of concept
- [ ] Evaluate race formats suitable for online play
- [ ] Add endianness-safe binary data infrastructure where needed

### Public 2.0 goals

- [ ] Ship an initial client/server multiplayer implementation
- [ ] Finalize replays as a fully supported feature

---

## Long-Term Exploration

These items are valuable but sit behind more practical work above.

- [ ] Modern rendering beyond the fixed-function OpenGL pipeline (Vulkan / modern OpenGL)
- [ ] Dynamic sky, weather, and day/night cycle
- [ ] Improved vegetation, shadows, and environment detail
- [ ] Procedural or in-game-assisted track creation
- [ ] In-game livery editing
- [ ] Animated driver and pit-crew models
- [ ] Dirt accumulation and broader visual-wear systems
- [ ] Free camera and improved spectating tools
- [ ] Force-feedback steering wheel support
- [ ] Story / career progression mode
- [ ] Traffic or free-roam modes
- [ ] Multi-core simulation
- [ ] Benchmark / screensaver-style modes
- [ ] Better opponent-set presets for human players
- [ ] Seamless handoff between human control and AI assistance

---

## Success Criteria

The roadmap is working if the project becomes **easier to build**, **easier to
understand**, **safer to redistribute**, and remains **useful for both players
and research-focused users**.

Last updated: 2026-04-06
