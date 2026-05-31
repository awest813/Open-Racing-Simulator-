# Content Compliance — Phase 0 Wave 1

Tracks removal/replacement of trademark-sensitive default content per [FORZA_VISION.md](FORZA_VISION.md).

## Priority replacements (ROADMAP)

| Asset | Location | Action |
|-------|----------|--------|
| `155-DTM` | `data/cars/models/155-DTM/` | Replace with generic DTM silhouette |
| `acura-nsx-sz` | `data/cars/models/acura-nsx-sz/` | Replace with generic sports car |
| `mc-larenf1` | search `data/cars/models/` | Replace or remove from default packs |
| `p406` | `data/cars/models/p406/` | Replace with generic hatch |

## Already flagged in README

- `data/cars/models/pw-*` — rally WRC liveries
- `data/cars/models/kc-*` — historic marques

## Wave 1 process

1. Remove listed models from default quick-race / career XML driver assignments.
2. Add `readme.txt` + `LICENSE` stub in each retired folder pointing to replacement.
3. Ship replacement meshes under fictional names in `data/cars/models/ors-*` (new prefix).
4. Audit `runtime/config` and `src/raceman` for hard-coded car names.

## Status

- [ ] Inventory all race XML references to priority assets
- [ ] Default install pack excludes `pw-*`, `kc-*`, priority list
- [ ] First replacement car (generic GT) for `acura-nsx-sz` slot
