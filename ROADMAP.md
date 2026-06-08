# Open Racing Simulator — Product & Technical Roadmap

This roadmap details the product direction, features, and engineering priorities for evolving **Open Racing Simulator** (ORS) into a premium, legally compliant, and moddable open-source Forza Motorsport-style racing simulator.

---

## 🌟 The Forza Motorsport-Class Vision
ORS aims to deliver a modern, accessible, yet deep racing game that feels responsive on controller/wheel and offers high-fidelity visual and audio feedback, while remaining:
1. **Open Source**: GPLv2 codebase, with curated CC-BY-SA (or compatible) default art assets.
2. **Moddable**: Core advantage; modular graphics, custom physics parameters, and robot AI written in data-driven structures (XML/JSON/GLTF).
3. **Research & Headless-Friendly**: High-speed, deterministic headless mode (`torcs -r`) for batch AI testing and reinforcement learning.

---

## 🛣️ Phased Development Roadmap

### Phase 1: Playable Widescreen Baseline (Current / Active)
* **Graphics & UI**:
  * Modernize legacyOptions, Exit, and Results constructors in C++ to use custom layouts.
  * Standardize UI canvases to `16:9` (`1280x720`) with aspect-ratio-aware letterboxing/pillarboxing scaling.
  * Expand display options with modern widescreen resolutions (e.g. 720p, 1080p, 1440p, 4K UHD).
  * Refactor texture mappings to render `16:9` assets without distortion or squishing.
* **Platform & Input**:
  * Adopt SDL2 as the default platform layer on Windows and Linux.
  * Abstract input systems to support modern steering wheel mapping and basic Force Feedback (FFB).
* **Audio**:
  * Clean up OpenAL context hooks, deprecating deprecated ALUT components.
  * Establish native support for loading OGG Vorbis sound effects.

### Phase 2: Graphics Modernization & Asset Pipeline
* **Renderer v2**:
  * Create a modular rendering module (`oglgraph2`) utilizing modern **OpenGL 3.3+** (or Vulkan).
  * Shift from legacy fixed-function pipeline matrices to customizable GLSL vertex and fragment shaders.
  * Support dynamic shadowing, post-processing filters (SSAO, bloom, blur), and dynamic skyboxes/day-night cycles.
* **GLTF 3D Asset Ingestion**:
  * Integrate standard **GLTF 2.0 (`.glb` / `.gltf`)** file loader utilities directly into the graphics module.
  * Retract the archaic AC3D (`.ac`) model formats from default car builds, allowing artists to export straight from Blender.
  * Formulate fictional generic car silhouettes under the `ors-*` prefix to replace legally sensitive trademark designs.

### Phase 3: Audio Depth & In-Game Radio
* **Dynamic Soundtrack Subsystem**:
  * Integrate an interactive `cRadioEngine` that parses music subfolders (`data/music/radio/`) into radio stations.
  * Construct on-screen HUD displays containing active track titles and station profiles.
  * Bind interactive keyboard and wheel shortcut keys to dynamically toggle volume and switch stations in-race.
* **3D Audio Enhancements**:
  * Configure OpenAL distance models, Doppler factor variables, and sound attenuation coefficients.

### Phase 4: Simulation Fidelity & Career Depth
* **Tyre & Damage Dynamics**:
  * Merge high-fidelity physical formulas from `simuv3` into the main simulation code.
  * Implement thermodynamic tire models calculating wear based on slip angles, velocity, and temperature.
  * Add configurable driving assists (adjustable traction control, stability control, and ABS).
* **Forza-Style Progression**:
  * Redraw the Rookie → Pro → Elite career path around credits, garage ownership, upgrades, and series championships.

### Phase 5: Connected Multiplayer
* **Netcode Pipeline**:
  * Deploy a dedicated server and peer-to-peer multiplayer synchronization layer using lightweight network libraries (e.g., `Lidgren` or `SDL_net`).
  * Ensure full simulation determinism for anti-cheat validation via playback audit logs.

---

## 💻 Code Hygiene & Refactoring

* **Eliminate Circular Dependencies**: Resolve core architectural loops (such as `raceman.h <-> simu.h` circular references) via interface abstractions.
* **Smart Pointers**: Replace raw pointer array allocations with `std::unique_ptr` and `std::shared_ptr` to govern lifetimes.
* **Logging System**: Convert standard prints to the consolidated `GfOut` logging utility.
* **Warning Auditing**: Zero-out MSVC/GCC compiler narrowing conversions in hot math loops.
