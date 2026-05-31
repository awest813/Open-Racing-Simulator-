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
The remaining default-driver references to retire are:

| Asset family | XML references | Notes |
|--------------|----------------|-------|
| `p406` | `src/drivers/inferno/inferno.xml`, `src/drivers/lliaw/lliaw.xml`, `src/drivers/tita/tita.xml`, plus mirrored `runtime/drivers/{inferno,lliaw,tita}/*.xml` | One bot entry per driver descriptor. Replace with a fictional hatch/sedan assignment. |
| `pw-*` | `src/drivers/berniw2/berniw2.xml`, `src/drivers/damned/damned.xml`, `src/drivers/olethros/olethros.xml`, plus mirrored `runtime/drivers/{berniw2,damned,olethros}/*.xml` | Rally/WRC entries (`pw-206wrc`, `pw-306wrc`, `pw-corollawrc`, `pw-evoviwrc`, `pw-focuswrc`, `pw-imprezawrc`). Replace with fictional rally cars or non-sensitive existing cars. |
| `kc-*` | `src/drivers/berniw3/berniw3.xml`, `src/drivers/inferno2/inferno2.xml`, plus mirrored `runtime/drivers/{berniw3,inferno2}/*.xml` | Historic marque-inspired entries (`kc-a110`, `kc-alfatz2`, `kc-coda`, `kc-corvette-ttop`, `kc-daytona`, `kc-db4z`, `kc-dino`, `kc-5300gt`, `kc-gt40`, `kc-p4`). Replace with fictional historic equivalents. |

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

## Status

- [x] Inventory all race XML references to priority assets
- [x] Default install pack excludes `pw-*`, `kc-*`, priority list
- [x] First replacement car (generic GT) for `acura-nsx-sz` slot
