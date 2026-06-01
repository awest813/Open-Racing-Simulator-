# Open Racing Simulator

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](COPYING)

**Open Racing Simulator** is a community-maintained fork of [TORCS](http://torcs.sourceforge.net/) 1.3.8 — a
moddable open-source racing simulator with AI drivers, editable car and track data,
runtime-loadable modules, and a headless mode well-suited for automation and
AI research.

The repository still carries a large amount of legacy TORCS structure and naming,
so you will see both "Open Racing Simulator" and "TORCS" throughout the codebase,
build files, and binaries. The two names refer to the same project.

---

## Table of Contents

1. [Features](#features)
2. [Repository Layout](#repository-layout)
3. [Architecture at a Glance](#architecture-at-a-glance)
4. [Building on Linux](#building-on-linux)
5. [Building on Windows](#building-on-windows)
6. [Running the Simulator](#running-the-simulator)
7. [Documentation and Learning Resources](#documentation-and-learning-resources)
8. [Testing and Debugging](#testing-and-debugging)
9. [Contributing](#contributing)
10. [Licensing and Content Notes](#licensing-and-content-notes)

---

## Features

- **Plugin-oriented architecture** — graphics, simulation, track loading, and robot
  drivers are all runtime-loaded modules that can be swapped independently.
- **Headless / console mode** — run races from the command line with no rendering
  overhead; ideal for batch testing, AI training, and automated experiments.
- **Built-in AI drivers** — 12 robot drivers of varying skill and style are included
  out of the box.
- **Rich car and track library** — 43 cars and 15 tracks ship with the simulator,
  covering oval, road, mixed, and dirt surfaces.
- **Data-driven configuration** — cars, tracks, race managers, and most simulator
  settings are expressed in XML; no recompile required to experiment.
- **Telemetry recorder** — in-session data capture for speed, suspension,
  braking, and other channels.
- **Adjustable brake balance** — configurable per session without restarting.
- **Learning-capable robots** — the `olethros` and `bt` drivers improve over time
  and persist acquired knowledge across sessions.
- **Doxygen API documentation** — full API docs can be generated from the source tree.
- **C++14 build** — the project has been upgraded to C++14 (`-std=c++14`) and
  progressively modernized toward current idioms.

---

## Repository Layout

```text
Open-Racing-Simulator-/
├── src/                  Engine code, interfaces, libraries, tools, and driver modules
│   ├── doc/              Architecture overview and developer notes
│   ├── interfaces/       Interface headers for all plugin types
│   ├── libs/             Shared utility libraries (TGF, math, robottools, learning…)
│   ├── modules/          Runtime-loadable plugins (graphics, simulation, track)
│   ├── drivers/          AI robot driver implementations
│   ├── linux/            Linux entry point and platform specifics
│   └── windows/          Windows entry point and platform specifics
├── data/                 Cars, tracks, sounds, menus, and runtime assets
├── doc/                  Manuals, tutorials, and generated API documentation output
├── test/                 Regression-style assets and utility test data
├── torcs_racing_board/   Web application used for the historic TORCS Racing Board
├── installer/            Packaging and installer resources
├── ROADMAP.md            Product and technical roadmap for the fork
└── CONTRIBUTING.md       Contributor setup guide and code conventions
```

---

## Architecture at a Glance

The simulator is organized around three major layers:

| Layer | Responsibility |
| :--- | :--- |
| **Orchestration** | Startup, menus, race-state management, and session flow |
| **Shared APIs and libraries** | XML parameter handling, math utilities, robot tools, portability layers, and common interfaces |
| **Runtime-loaded modules** | Graphics, simulation, track, and robot plugins loaded via `GfModLoad()` |

The three major plugin slots are:

- **Graphics** (`ssggraph`) — OpenGL 1.x + PLIB SSG rendering with OpenAL audio.
- **Simulation** (`simuv2` / `simuv3`) — custom physics engine with simplified
  Pacejka tire model and configurable per-car parameters.
- **Robot** — any number of robot modules can be loaded simultaneously; each
  module may drive up to 10 cars.

For a deeper walkthrough, start with:

- [`src/doc/architecture.md`](src/doc/architecture.md)
- [`src/interfaces/`](src/interfaces/)
- [`src/libs/raceengineclient/`](src/libs/raceengineclient/)
- [`src/modules/`](src/modules/)

---

## Building on Linux

### Prerequisites

On Debian / Ubuntu:

```bash
sudo apt-get install \
  build-essential autoconf automake \
  libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev \
  libplib-dev libopenal-dev libalut-dev \
  libvorbis-dev zlib1g-dev libpng-dev libx11-dev libxrandr-dev
```

Other distributions will need the equivalent packages for:

| Dependency | Minimum version |
| :--- | :--- |
| GCC / G++ | any version with C++14 support |
| GNU Make | — |
| PLIB | 1.8.3 or newer |
| OpenGL + GLU | — |
| GLUT / FreeGLUT | — |
| OpenAL + ALUT | — |
| libvorbisfile | — |
| zlib + libpng | — |
| X11 + XRandR dev headers | — |

### Build steps

```bash
./configure
make
make install
make datainstall
```

### Useful configure flags

| Flag | Effect |
| :--- | :--- |
| `--enable-debug` | Enable debug symbols and assertions |
| `--disable-xrandr` | Disable XRandR (use if it causes issues on your system) |

### API documentation

```bash
make doc          # requires Doxygen
# open doc/manual/api/index.html
```

---

## Building on Windows

**Modern build (CMake - Recommended):**

```powershell
# Create a build directory
cmake -S . -B build_x86
# Build the project
cmake --build build_x86 --config Release
# Run the simulator from the bin directory
./build_x86/bin/wtorcs.exe
```

**Legacy build:**

The repository still carries older Visual Studio project files and helper batch scripts.

---

## Running the Simulator

| Command | Description |
| :--- | :--- |
| `torcs` | Start the interactive GUI application |
| `torcs -r <race-config.xml>` | Run a race from the command line (no GUI) |
| `torcs -d` | Enable debug stack-trace support |
| `torcs -g` | Run under Valgrind via the launcher script |
| `torcs -k` | Keep modules loaded (helpful for Valgrind call stacks) |
| `torcs -s` | Disable multitexturing for older hardware |

The `-r` console mode runs without rendering, so simulation time is not
synchronized with real time — useful for fast batch experiments and AI training.

---

## Documentation and Learning Resources

| Resource | Location |
| :--- | :--- |
| Architecture overview | [`src/doc/architecture.md`](src/doc/architecture.md) |
| Robot development tutorial | [`doc/tutorials/robot/README`](doc/tutorials/robot/README) |
| API reference | `make doc` → `doc/manual/api/index.html` |
| Track-gen regression notes | [`test/trackgen/README`](test/trackgen/README) |
| Contributor guide | [`CONTRIBUTING.md`](CONTRIBUTING.md) |
| Project roadmap | [`ROADMAP.md`](ROADMAP.md) |
| Forza-class vision (phased) | [`doc/planning/FORZA_VISION.md`](doc/planning/FORZA_VISION.md) |
| Phase 0 checklist (complete) | [`doc/planning/PHASE0.md`](doc/planning/PHASE0.md) |
| Phase 1 checklist (active) | [`doc/planning/PHASE1.md`](doc/planning/PHASE1.md) |
| Headless regression | [`test/regression/README.md`](test/regression/README.md) |

If you are working on robots, race-manager XML, or simulation changes, the
architecture document is the best starting point.

---

## Testing and Debugging

This codebase does not yet have a full automated unit-test suite. Validation is
done through:

- Local builds
- Command-line race runs (`torcs -r`)
- Targeted regression assets in `test/`
- Valgrind-assisted debugging on Linux

For simulation or AI work, prefer reproducible command-line sessions before
testing through the interactive UI.

---

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for:

- Development environment setup
- Code conventions
- How to submit a pull request
- Bug reporting guidelines

Contributions to robots, simulation, tooling, documentation, and content
(with clear GPL-compatible licensing) are all welcome.

---

## Licensing and Content Notes

Code in this repository is distributed under the **GPL v2**; see [`COPYING`](COPYING)
for the full license text.

Some included artwork and content have separate licensing terms. The following
asset directories contain content with potential trademark or redistribution
concerns:

- `data/cars/models/pw-*` — rally-car content
- `data/cars/models/kc-*` — historic car content

Check the `readme.txt` files shipped in those asset directories before
redistributing or repackaging. Replacement of trademark-sensitive content is a
tracked item on the roadmap.
