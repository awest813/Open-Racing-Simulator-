# Open Racing Simulator — Roadmap

This document tracks planned features, improvements, and long-term goals for the
Open Racing Simulator (based on TORCS v1.3.8). Items are grouped by release milestone
and roughly ordered by priority within each milestone.

---

## Current Release: v1.3.8 "worn & blown"

Status: **active maintenance**

- [x] Tire temperature / wear model
- [x] Caster adjustable via setup
- [x] Suspension slow/fast threshold configurable
- [x] 3-D suspension calculations (replaced 2.5-D)
- [x] Hidden Valley track added
- [x] TRB race configuration generator
- [ ] Car adjustments for tire wear / performance balance
- [ ] Improved ranking with penalties for undamaged-but-out-of-fuel cars
- [ ] Brake balance adjustable during a race session
- [ ] Curb sound
- [ ] Dynamic track surface (rubber build-up, etc.)
- [ ] Wind & temperature effects

---

## v1.3.9 "analysed" — Telemetry & Data Analysis

Goal: give drivers and robot developers insight into simulation data.

- [ ] Data recorder ("Telemetry") — capture per-frame vehicle state to file
- [ ] Data analyser — high/low-pass filtering, lap comparison, overlays
- [ ] Speed / shock-absorber telemetry channels
- [ ] Replay system (initial prototype)

---

## v1.3.10 "ruled" — Race Rules

Goal: add configurable rules without requiring a v1.4 API break.

- [ ] Timed races (e.g. 24-hour endurance format)
- [ ] Additional penalty / rule modes
- [ ] Fix side-entering/exiting pit-lane repeated-violation bug

---

## v1.3.11 "tutored" — Tooling & Documentation

Goal: lower barrier to entry for new robot developers.

- [ ] Starting / race modes (multi-class support for TRB)
- [ ] Visual Studio project update (VS 2015+)
- [ ] Robot Tutorial refresh

---

## v1.3.12 "managed" — Configuration & GUI

- [ ] Document race-manager XML format
- [ ] Expose hidden settings in GUI (e.g. button masking)
- [ ] GUI for event blacklisting

---

## v1.3.13–v1.3.14 "artistic / everywhere" — Content & Platform

**Content**
- [ ] In-game track generation wizard (themed)
- [ ] In-game car livery designer (themed)

**Physics / Simulation**
- [ ] Merge simuv3 improvements into simuv2
- [ ] ABS implementation review and merge (Wolf-Dieter patch)
- [ ] 4WD analysis / patch review and merge
- [ ] Differential(s) adjustable during a race session
- [ ] Remove sound-property calculation from wheel.cpp

**Platform**
- [ ] macOS build review and merge
- [ ] MorphOS changes review and merge
- [ ] Remove circular dependency between `raceman.h` and `simu.h`

---

## v1.3.15 "stable" — Maintenance Only

- [ ] Compiler and library adoption fixes
- [ ] Compatible content updates
- [ ] Move new feature development to v1.4 branch

---

## v1.4.x — API Modernisation

Goal: clean break for incompatible improvements.

**Robot API**
- [ ] Optional `askfeatures` / `callonce` / `grid` / `postprocess` callbacks
- [ ] Identify robots by name only (remove integer-index dependency)
- [ ] Allow a robot module to carry an arbitrary number of drivers (remove static limit of 10)

**Audio**
- [ ] Update OpenAL to 1.1; remove deprecated ALUT calls
- [ ] Replace WAV sounds with OGG Vorbis
- [ ] Fix sound in split-screen multiplayer

**Graphics**
- [ ] Apply SDL 2.0 patch (replaces GLUT windowing)
- [ ] Skid marks / shadows using stencil masking
- [ ] Phong specular highlights / shadow occlusion
- [ ] Skid-mark persistence moved to simulation layer (remove frame-rate dependency)
- [ ] Store entire graphics-engine state in a context struct (enables telemetry overlay)
- [ ] Reduce dynamic memory allocation during rendering

**Physics / Simulation**
- [ ] Brake / engine wear model with cooling simulation
- [ ] Track wall properties
- [ ] Track extensions: crossings, split/join segments, variable width

**Content**
- [ ] Pace car and track trucks to remove wrecks (replace virtual crane)

**Compliance — replace or remove trademarked content**
- [ ] Replace 155-DTM with car10-trb1
- [ ] Replace acura-nsx with car9-trb1
- [ ] Replace mc-larenf1 with car10-trb1
- [ ] Replace p406 with car1-trb4
- [ ] Rework buggy and baja-bug models
- [ ] Replace rally cars
- [ ] Remove invalid geometry from tracks
- [ ] Convert force units internally from lbs to lbf

---

## v1.9.x — Networking Pre-Release (no public release)

Goal: design and prototype networking before committing to a 2.0 API.

- [ ] Network architecture design (how to embed into simulation loop)
- [ ] Networking prototype / proof-of-concept
- [ ] Gaming modes suitable for online races
- [ ] Cockpit inside-view camera
- [ ] Infrastructure for endianness-independent binary data files

---

## v2.0.0 — Networking

- [ ] Initial networked multiplayer (client/server)
- [ ] Replays (final implementation)

---

## Long-Term / Aspirational

These items have no fixed milestone; they require significant design work or
depend on earlier milestones.

- [ ] Modern rendering: OpenGL 3.x / OpenGL ES 2 or Vulkan (replace PLIB SSG)
- [ ] Shadowmapped / stencilled dynamic car shadows
- [ ] 3-D grass and vegetation
- [ ] Dynamic sky (time of day, weather)
- [ ] Weather model (rain, wind gusts)
- [ ] Dynamic day/night cycle with car headlights
- [ ] Animated driver model
- [ ] Dirt accumulation on cars
- [ ] Pit crew animations
- [ ] Free-roam terrain
- [ ] Procedural track generation
- [ ] Separate pit path and Y-segment support
- [ ] Force-feedback steering wheel support
- [ ] Story / career mode
- [ ] Traffic simulation mode
- [ ] SMP (multi-core) simulation core
- [ ] TORCS as a benchmark / screensaver mode
- [ ] Free camera (mouse + keyboard) during replay/spectating
- [ ] Opponent sets for human players (e.g. 20 open-wheel cars)
- [ ] More driving aids + seamless AI hand-off and take-back

---

## Code Quality & Tech Debt

These are ongoing concerns that should be addressed incrementally across all milestones.

- [ ] Migrate from pre-C++11 idioms to modern C++ (smart pointers, `nullptr`, `auto`, range-`for`)
- [ ] Replace `malloc`/`free`/`FREEZ` macro with RAII types
- [ ] Replace C-style casts with `static_cast` / `reinterpret_cast`
- [ ] Replace `printf`-based logging with a structured logger
- [ ] Replace remaining `strcpy`/`strcmp` calls with `std::string`
- [ ] Add a unit-test suite (start with physics utilities and XML param handling)
- [ ] Resolve circular dependency `raceman.h` ↔ `simu.h`
- [ ] Move `berniw` / `bt` spline code into `src/libs/math` (shared library)
- [ ] Eliminate `glGet*` calls inside the simulation loop
- [ ] Refactor `trackgen` to eliminate left/right duplication
- [ ] Replace remaining PLIB SSG usage with a maintained rendering library

---

*Last updated: 2026-03-26*
