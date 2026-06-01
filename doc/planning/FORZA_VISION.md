# Forza-Class Vision — Phased Roadmap

This document is the **product roadmap** for evolving [Open Racing Simulator](https://github.com/awest813/Open-Racing-Simulator-) into a credible, redistributable, moddable **open-source Forza Motorsport–class** experience. It complements the engineering-focused [`ROADMAP.md`](../../ROADMAP.md) and [`phase2.md`](phase2.md).

**Last updated:** 2026-05-31

---

## 1. Vision

Deliver a motorsport game that feels modern and accessible like Forza Motorsport, while remaining:

- **Open source** (GPLv2 engine; clear licenses on content)
- **Moddable** (cars, tracks, robots as data + plugins)
- **Research-friendly** (headless `torcs -r`, deterministic regression)

We are **not** cloning Microsoft’s product. We target fictional manufacturers, generic liveries, and community-scale production — with depth in tuning, career, and (eventually) online racing.

---

## 2. Target pillars

| Pillar | Forza (reference) | Our target |
|--------|-------------------|------------|
| Feel | Fun at the limit; assists; wheels | Tunable assists + optional sim depth; FFB |
| Cars | Large licensed roster | 50–150 **generic** cars, clear classes, mod pipeline |
| Tracks | Real + fictional circuits | 30–50 tracks; community imports |
| Progression | Career, credits, upgrades | Extend existing career → garage, economy, series |
| Presentation | Modern graphics, weather, UI | Renderer v2 + weather + new front-end |
| Competition | Online, replays, rivals | Deterministic replay → netcode (Phase 5) |
| Modding | Limited | **Core advantage** — data-driven everything |

### Non-goals

- Licensed real-world marques/tracks in the default install
- AAA art budget or photorealism on day one
- Cloud “Drivatar” ML (lightweight telemetry profiles instead)

---

## 3. Current baseline

**Strengths:** plugin architecture (`simuv2`/`simuv3`, `ssggraph`, track loaders, 12 AI robots), headless mode, XML content, telemetry, career scaffold (`careermenu.cpp`: Rookie → Pro → Elite), active modernization (C++14, CMake, logging).

**Gaps:** OpenGL 1.x presentation; simplified physics; ~43 cars with some trademark risk; menu-only career; no multiplayer; limited wheel/FFB; replay not shipped.

---

## 4. Architecture direction

```text
Presentation (UI, Renderer v2, Audio)
        ↓
Game layer (Progression, Rules, Replay)  ← new, on race engine
        ↓
Core (Race engine, Race manager XML, Telemetry)  ← existing
        ↓
Simulation plugins (Physics, Track, Robots)
        ↓
Platform (SDL2, Networking in Phase 5)
```

**Rules**

1. Do not break headless / research workflows.
2. Version robot, graphics, and replay APIs before multiplayer.
3. Determinism before netcode (regression suite is mandatory).
4. Content pipeline beats one-off assets.

---

## 5. Phased delivery

### Phase 0 — Foundation (complete)

**Goal:** Safe foundation for all later phases.

| Item | Status | Notes |
|------|--------|-------|
| Finish CMake (exe, graphics, drivers, asset install) | Done | See [`phase2.md`](phase2.md) |
| Headless regression suite | Done | [`test/regression/`](../../test/regression/) |
| Replay format RFC | Done | [`REPLAY_FORMAT.md`](REPLAY_FORMAT.md) |
| SDL2 window/input path | PoC | [`SDL2_MIGRATION.md`](SDL2_MIGRATION.md) |
| Break `raceman.h` ↔ `simu.h` cycle | Done | `simuitf.h` |
| Content compliance wave 1 | Done | [`CONTENT_COMPLIANCE.md`](CONTENT_COMPLIANCE.md) |
| CI (CMake Linux + regression) | Done | GitHub Actions |

**Exit criteria:** Met — see [`PHASE0.md`](PHASE0.md).

---

### Phase 1 — Playable modern sim (active)

**Goal:** Pad/wheel playable; contemporary look and feel.

**Checklist:** [`PHASE1.md`](PHASE1.md)

- SDL2 default; wheel mapping; basic FFB
- Graphics pass A (GL 3.3+ or parallel modern renderer module)
- UI pass A (main menu, quick race, settings; career hub wiring)
- Physics pass A (`simuv3` merge, wear, ABS/4WD if stable)
- Audio (OGG, OpenAL cleanup)
- Rules (penalties, track limits)

**Targets:** 1080p60 mid hardware; 20 legal cars, 12 tracks; career Rookie→Elite in new UI.

---

### Phase 2 — Motorsport depth

- Homologation classes, performance index, multi-class races
- In-game tuning / garage presets
- Track features (variable width, dynamic rubber/temperature)
- Weather v1 (grip model; visuals optional)
- AI skill tiers and lightweight rival profiles
- **60+ cars, 25+ tracks** via pipeline
- Replay v1 (full session from deterministic log)

---

### Phase 3 — Career & progression

Extend [`careermenu.cpp`](../../src/libs/raceengineclient/careermenu.cpp):

- Credits economy, garage ownership, upgrade parts (XML-driven)
- Series calendar map (5–8 tiers)
- License tests (Bronze / Silver / Gold)
- Named rival AI profiles

**Exit criteria:** 15+ hour solo path; versioned `career.xml` saves.

---

### Phase 4 — Spectacle & sandbox

- Day/night, headlights
- Photo / free camera
- Livery editor (template + import)
- Track editor assist (`trackgen` + CI validation)
- Benchmark / hot-lap mode

---

### Phase 5 — Connected motorsport (2.0)

- LAN / dedicated server; input sync
- Lag compensation; replay audit for anti-cheat
- Community servers; optional ranked ladder (extend `torcs_racing_board` or successor)

**Gate:** Phase 0 determinism + stable APIs only.

---

## 6. Content strategy

| Milestone | Cars | Tracks |
|-----------|------|--------|
| Phase 1 | 20 legal | 12 |
| Phase 2 | 60 | 25 |
| Phase 3+ | 100+ (mods) | 40+ (mods) |

- Default install: **fictional** brands only (`kc-*`, `pw-*` retired from default pack).
- Templates: `data/cars/models/_template/`, track readme + license per asset.
- Curated “official” pack in-repo; community pack optional separate repo.

---

## 7. Success metrics

| Metric | Target |
|--------|--------|
| New player → first race | < 5 minutes |
| Wheel + gamepad | Fully playable without keyboard |
| Career Rookie completion | > 30% of players |
| Headless lap reproducibility | ±0.1% same machine |
| Mod adoption | > 50% installs add community content |

---

## 8. 30-day vertical slice (Phase 0 → 1 bridge)

1. One car, **forza** track, AI-only headless regression green.
2. Record baseline via `test/regression/record_baseline.sh`.
3. Replace top 4 trademark cars in default `data/` (ROADMAP list).
4. SDL2 audit document + first window creation on SDL2 path.
5. Career Rookie race runnable end-to-end (existing XML + menus).

---

## 9. Related documents

| Document | Role |
|----------|------|
| [`ROADMAP.md`](../../ROADMAP.md) | Engineering tracks 1.3 → 2.0 |
| [`phase2.md`](phase2.md) | CMake / SDL2 / CI detail |
| [`PHASE0.md`](PHASE0.md) | Phase 0 foundation checklist (complete) |
| [`PHASE1.md`](PHASE1.md) | Active Phase 1 checklist |
| [`REPLAY_FORMAT.md`](REPLAY_FORMAT.md) | Replay / ghost binary format |
| [`test/regression/README.md`](../../test/regression/README.md) | Headless regression usage |
| [`src/doc/architecture.md`](../../src/doc/architecture.md) | Plugin architecture |

---

## 10. Governance

- **Engine:** physics, determinism, API versions  
- **Graphics:** renderer migration, asset standards  
- **Game design:** career, classes, rules  
- **Content:** pipeline + licensing  
- **DevOps:** CMake CI, regression, releases  

**Release train:** quarterly tags; `main` always builds; LTS branch for modders.

**License:** GPLv2 code; CC-BY-SA (or compatible) art packs with per-folder `LICENSE` / `readme.txt`.
