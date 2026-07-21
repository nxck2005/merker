# merker

Real-time Keplerian 2-body orbit simulator in C++23, with a live raylib/OpenGL 3D visualiser.

A satellite is integrated around a parent body (Earth) and rendered in real time — textured
Earth, skybox, a fading orbit trail, and a HUD showing position, velocity, epoch, and
collision state. Optional CSV output feeds Python/Jupyter trajectory analysis.

## Features
- Newtonian point-gravity integration with selectable integrators (energy-conserving Verlet
  by default, plus Euler).
- Preconfigured bodies: Earth (parent), ISS and Moon (satellites), with arbitrary orbital
  parameters supported.
- Threaded architecture — physics runs on a worker thread, decoupled from the render loop.
- Live 3D visualisation: first-person camera, textured Earth, cubemap skybox, orbit trail.
- Optional CSV trajectory dump for offline analysis.

## Requirements
- C++23 compatible compiler (e.g. GCC 13+ or Clang 16+)
- [xmake](https://xmake.io) build system (fetches GLM and raylib automatically)
- For analysis: Python 3.11+ (managed with [uv](https://docs.astral.sh/uv/))

## Build and run

The project lives in the `merker/` subdirectory:

```bash
cd merker
xmake        # build (fetches dependencies on first run)
xmake run    # launch the visualiser
```

Resource paths are resolved relative to the build directory, so launch via `xmake run`
rather than invoking the binary directly.

## Controls

The view starts pointed at Earth. To move around:

| Input                     | Action                     |
| ------------------------- | -------------------------- |
| Left mouse button + drag  | Look around                |
| `W` / `S`                 | Move forward / backward    |
| `A` / `D`                 | Strafe left / right        |
| `Space` / `Left Shift`    | Move up / down             |
| `Esc`                     | Quit                       |

## Ground track

The **Render Orbit** button (top-right of the main window) opens a second window
showing the satellite's ground track over an equirectangular map of Earth. A slider
sets how many orbits to draw; successive orbits are overlaid and drift westward as
Earth rotates beneath the orbit, updating live as you move the slider.

The ground-track window is a separate process (raylib is single-window), launched as
`merker --groundtrack`. It re-derives the orbit deterministically, so it needs no data
from the main window.

## Configuration

Simulation setup lives in `src/main.cpp` (parent/satellite selection, timestep, CSV
toggle). Bodies and their initial state vectors are defined in `src/bodies.cpp`.

## Visualisation & analysis

CSV dumping is off by default. Enable `dumpToCSV` in `src/main.cpp` to write `orbit.csv`,
then plot trajectories with the scripts in `src/scripts/`:

```bash
uv sync
uv run python src/scripts/parse_orbit.py    # writes orbit_pos.png / orbit_vel.png
```

`notebooks/analyse.ipynb` provides an interactive alternative.
