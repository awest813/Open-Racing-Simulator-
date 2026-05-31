# Replay Format RFC (v0.1)

**Status:** Draft — Phase 0 design  
**Goal:** Deterministic record/playback for ghosts, rivals, and future multiplayer audit.

---

## 1. Requirements

1. **Deterministic playback** on the same build, track, car, and robot modules.
2. **Compact** enough for full-race storage (target &lt; 5 MB for 10-lap AI race).
3. **Versioned** header for forward compatibility.
4. **Headless-friendly** — record without graphics module loaded.
5. **Extensible** — optional full state snapshots for debug; not required v1.

---

## 2. File layout

```text
[Header 64 bytes]
[Tick index table]        — optional, for seek
[Frame records ...]
[Footer checksum 4 bytes] — CRC32 of payload
```

### 2.1 Header (fixed 64 bytes, little-endian)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 4 | Magic `ORSR` (0x4F525352) |
| 4 | 2 | Format version (major) |
| 6 | 2 | Format version (minor) |
| 8 | 4 | Build fingerprint (hash of git commit truncated) |
| 12 | 4 | Tick rate Hz (e.g. 1000 / RCM_MAX_DT_SIMU) |
| 16 | 4 | Car count |
| 20 | 4 | Frame count |
| 24 | 32 | SHA-256 truncated track+car+robot config blob |

Remaining header bytes reserved (zero).

### 2.2 Frame record (per simulation tick)

| Field | Type | Description |
|-------|------|-------------|
| `dt` | uint16 | Tick duration in 0.1 ms units |
| `inputs[]` | per car | See §3 |
| `state_hash` | uint32 | Optional FNV-1a of minimal state vector |

If `state_hash` mismatches on playback, log warning (non-fatal in v0.1).

---

## 3. Per-car input payload (v0.1)

Maps to robot/human control channels already in simulation:

| Byte | Mask | Channel |
|------|------|---------|
| 0 | 0x01 | Accelerator |
| 0 | 0x02 | Brake |
| 0 | 0x04 | Gear up |
| 0 | 0x08 | Gear down |
| 1 | — | Steer (-127..127) |
| 2 | 0x01 | Clutch |
| 2 | 0x02 | Handbrake |

Future: analog steer as int16, flags for assists.

---

## 4. Playback rules

1. Load same race manager XML + track + car indices as recording.
2. Disable robot AI; feed recorded inputs via shim implementing `rbDrive`.
3. Fixed timestep only — no catch-up if frame drops.
4. Random seeds for robots must be stored in header extension v0.2.

---

## 5. Implementation phases

| Phase | Deliverable |
|-------|-------------|
| 0.1 | This RFC + file naming: `replay-<track>-<timestamp>.orsr` |
| 0.2 | Record hook in race engine after `SimUpdate` |
| 0.3 | Ghost car renderer using playback shim |
| 1.0 | Seek table + compression (zstd) |

---

## 6. Open questions

- Store robot internal state vs inputs-only (inputs-only preferred for size).
- Endianness: little-endian only for v0.1; document for future network.
- Integration with telemetry CSV exporter (separate files, shared session id).

---

## 7. References

- Race engine: `src/libs/raceengineclient/racestate.cpp`
- Console races: `ReRunRaceOnConsole()` in `raceinit.cpp`
- Regression config: `test/regression/regression-race.xml`
