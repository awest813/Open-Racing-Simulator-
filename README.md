# Open Racing Simulator (ORS)

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](COPYING)

**Open Racing Simulator** is an open-source, community-driven racing simulator built on a highly modular C++ architecture. Evolved from [TORCS](http://torcs.sourceforge.net/), the project is being modernized into a credible, moddable, **open-source Forza Motorsport-style motorsport experience** with advanced graphics, custom asset pipelines, dynamic audio, and deep career modes.

---

## 🌟 The Product Vision

Deliver a motorsport simulation that feels modern and accessible (featuring assists, wheel mapping, and force feedback) while remaining:
* **Fully Open Source**: GPLv2 codebase, with CC-BY-SA artwork.
* **Extremely Moddable**: Core advantage; cars, tracks, and drivers are fully data-driven.
* **Research & Headless Friendly**: Command-line simulation mode (`torcs -r`) for batch AI testing and reinforcement learning.

For detailed design specifications and deliverables, see:
* **[ROADMAP.md](ROADMAP.md)**: Product & technical roadmap.
* **[Mega Planning Blueprint](doc/planning/FORZA_VISION.md)**: Forza-class vision and phased checksheets.

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
├── doc/                  Manuals, tutorials, and planning roadmaps
│   └── planning/         Phased feature delivery blueprints and specifications
├── test/                 Regression-style assets and utility test data
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
* **Graphics** (`ssggraph`) — Custom OpenGL rendering with OpenAL audio.
* **Simulation** (`simuv2` / `simuv3`) — Advanced physics engine with tyre wear and thermal dissipation equations.
* **Robot** — Dynamic AI driver plugins supporting advanced learning and path-finding.

---

## Building on Windows (CMake)

**Prerequisites:** Visual Studio (with C++ development workloads) and CMake.

```powershell
# Create a build directory
cmake -S . -B build_x86

# Build the project
cmake --build build_x86 --config Release

# Run the simulator from the bin directory
./build_x86/bin/wtorcs.exe
```

---

## Running the Simulator

| Command | Description |
| :--- | :--- |
| `torcs` | Start the interactive GUI application |
| `torcs -r <race-config.xml>` | Run a race from the command line (headless, no GUI) |
| `torcs -d` | Enable debug stack-trace support |

---

## Licensing and Content Notes

Code in this repository is distributed under the **GPL v2**; see [`COPYING`](COPYING) for the full license text.
Default assets are curated to exclude trademarked real-world marques, shipping instead under fictional generic branding (`ors-*`).
