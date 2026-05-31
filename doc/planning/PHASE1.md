# Phase 1 — Playable Modern Sim

Active work stream after [Phase 0](PHASE0.md). See [FORZA_VISION.md](FORZA_VISION.md) for goals and targets.

**Goal:** Pad/wheel playable; contemporary look and feel.

## Platform & input

- [ ] SDL2 default build path on Linux (GLUT legacy optional until removal)
- [ ] Wheel mapping and basic force feedback
- [ ] High-DPI and resizable window validation on SDL2 path

## Graphics & UI

- [ ] Graphics pass A (GL 3.3+ or parallel modern renderer module)
- [ ] UI pass A (main menu, quick race, settings)
- [ ] Career hub wiring to existing `careermenu.cpp` (Rookie → Elite)

## Simulation & audio

- [ ] Physics pass A (`simuv3` merge, wear, ABS/4WD if stable)
- [ ] Audio (OGG, OpenAL cleanup)
- [ ] Rules (penalties, track limits)

## Content

- [ ] 20 legal fictional cars in default pack
- [ ] 12 tracks in default quick-race rotation
- [ ] Replace remaining priority trademark meshes with `ors-*` assets (see [CONTENT_COMPLIANCE.md](CONTENT_COMPLIANCE.md))

## Validation

- [ ] Career Rookie race runnable end-to-end in new UI
- [ ] 1080p60 on mid-tier hardware (smoke target)
- [ ] Regression suite stays green after each milestone

---

**Related:** [SDL2_MIGRATION.md](SDL2_MIGRATION.md), [REPLAY_FORMAT.md](REPLAY_FORMAT.md), [test/regression/README.md](../../test/regression/README.md)
