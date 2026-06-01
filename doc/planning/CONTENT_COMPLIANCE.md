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

## Race XML inventory

Checked race-driver descriptors with:

```bash
rg 'attstr name="car name" val="(155-DTM|acura-nsx-sz|mc-larenf1|p406|pw-[^"]+|kc-[^"]+)"' \
  --glob '*.xml'
```

No race XML currently assigns `155-DTM`, `acura-nsx-sz`, or `mc-larenf1`.

Driver descriptors in `src/drivers/` and `runtime/drivers/` now assign fictional
default-pack cars instead of `p406`, `pw-*`, and `kc-*` (see replacement table
below).

## Default pack exclusion

Default packaging now keeps these model directories in the source tree but
excludes them from generated runtime/install outputs:

- CMake portable runtime (`torcs_runtime_assets`) removes retired car model
  directories after syncing `data/`.
- `cmake --install` excludes `155-DTM`, `acura-nsx-sz`, `mc-larenf1`, `p406`,
  `pw-*`, and `kc-*` under `data/cars/models/`.
- Legacy Make data recursion filters those same model directories out of
  `DATASUBDIRS`/`PKGSUBDIRS`, and the default package list no longer builds the
  retired Patwo/KCendra car packs.

## Replacement assets

| Replacement | Source/provenance | Replaces | Notes |
|-------------|-------------------|----------|-------|
| `ors-gt` | Derived from the fictional `car8-trb1` model (Free Art License metadata in `readme.txt`) | `acura-nsx-sz` default-pack slot | Added under the new `ors-*` prefix and assigned to `Track-RWD-GrB` so the retired Acura-branded model can stay excluded from default packages. |

## Driver XML replacements (wave 1)

| Retired | Replacement | Used by |
|---------|-------------|---------|
| `p406` | `car1-stock2` | inferno, lliaw, tita |
| `pw-focuswrc` | `baja-bug` | berniw2, damned |
| `pw-206wrc` | `buggy` | berniw2, damned |
| `pw-306wrc` | `car1-stock1` | berniw2, damned |
| `pw-evoviwrc` | `car1-stock2` | berniw2, damned |
| `pw-imprezawrc` | `car2-trb1` | berniw2, damned |
| `pw-corollawrc` | `car3-trb1` | berniw2, damned, olethros |
| `kc-dino` … `kc-5300gt` | `car8-trb1` … `ors-gt` | berniw3, inferno2 |

## Status

- [x] Inventory all race XML references to priority assets
- [x] Default install pack excludes `pw-*`, `kc-*`, priority list
- [x] First replacement car (generic GT) for `acura-nsx-sz` slot
- [x] Retire `p406`, `pw-*`, `kc-*` from default driver descriptors
- [x] Retirement stubs for `p406` and `155-DTM` model folders
