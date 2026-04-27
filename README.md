# merker

Keplerian 2-body orbit sim written in C++.

## Features
- Support for arbitrary orbital parameters.
- CSV output for trajectory analysis.

## Requirements
- C++23 compatible compiler (e.g., GCC 13+ or Clang 16+)
- xmake build system
- GLM (managed by xmake)

## Build and Run
To build and run the simulation:
```bash
cd merker
xmake
xmake run
```
The simulation outputs data to `orbit.csv`.

## Visualization
Trajectories can be visualized using Python scripts in the `src/scripts` directory.
