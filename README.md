# Kralica

Kralica is a modular C++ N-body simulation framework with configurable force fields, multiple integrators, CSV logging, and Python-based animation.

## Animation

The simulation output can be rendered as an MP4 animation from `./build/data.csv`.

<p align="center">
  <video src="./build/animation.mp4" controls width="720"></video>
</p>

If your Markdown renderer does not support embedded video, use the link below:

- [Watch/download `animation.mp4`](./build/animation.mp4)

## Features

### Core Simulation
- N-body simulation framework with configurable timestep and end time.
- Modular architecture: bodies, force fields, integrators, and simulation are separated.
- Supports multiple force fields through `CompositeField`.
- Validates system size and integrator before simulation.
- Tracks current simulation time.
- Logs body positions to CSV for visualization.

### Vector Mathematics
- Header-only `Vector<T, Dim>` template.
- Supports arbitrary dimensions `Dim >= 1` and arithmetic types.
- Type aliases:
  - `Vector2i`, `Vector2f`, `Vector2d`
  - `Vector3i`, `Vector3f`, `Vector3d`
- Operations:
  - Addition, subtraction, scalar multiplication, scalar division
  - Compound assignments: `+=`, `-=`, `*=`, `/=`
  - Equality and approximate equality
  - Dot product
  - L2 norm, L1 norm, infinity norm
  - Normalization
  - 3D cross product
- `std::formatter` support for `std::format`, `std::print`, and `std::println`.
- Compile-time vector tests using `static_assert`.

### Bodies
- `Body` class stores:
  - Position
  - Velocity
  - Mass
- Getter and setter methods for all properties.
- Mass validation: throws `std::invalid_argument` if mass is not positive.

### Force Fields
- Abstract `ForceField` interface:
  - `compute(const std::vector<Body>&, std::vector<Vector2d>&) const`
- `GravityForce`:
  - Newtonian gravity
  - Softening parameter `R_SOFT_GRAV` to avoid singularities
- `LennardJonesForce`:
  - Lennard-Jones potential
  - Softening parameter `R_SOFT_LJ`
  - Cutoff distance `R_CUTOFF_LJ`
- `CompositeField`:
  - Combines multiple force fields
  - Computes each field in sequence

### Integrators
- Abstract `Integrator` interface:
  - `step(std::vector<Body>&, std::vector<Vector2d>&, const CompositeField&, double) const`
- `EulerIntegrator`:
  - Basic Euler integration
- `VerletIntegrator`:
  - Velocity Verlet integration
  - Periodic boundary wrapping using `wrap()`
  - Uses `BOX_SIZE` for boundary conditions

### Simulation
- `Simulation` class manages:
  - Bodies
  - Accelerations
  - Integrator
  - Composite force field
  - Timestep
  - Current time
- Methods:
  - `step()`
  - `run(double end_time)`
  - `run_and_log(double end_time, const std::string& file_name)`
  - `print_statistics()`
  - `log_statistics(std::ofstream&)`
- `run_and_log` writes CSV columns such as:
  - `x0,y0,x1,y1,...`

### Example Simulation
- `main.cpp` creates a 5-body system:
  - One central body with mass `500`
  - Four orbiting bodies with mass `1`
- Uses:
  - `GravityForce`
  - `VerletIntegrator`
  - `dt = 1e-3`
  - `end_time = 2.00`
- Writes output to `./build/data.csv`.

### Visualization
- `visualization.py` reads `./build/data.csv`.
- Uses:
  - `numpy`
  - `pandas`
  - `matplotlib`
- Creates an animated scatter plot of all bodies.
- Saves animation to `./build/animation.mp4`.
- Uses `ffmpeg` writer for faster MP4 export.

### Build and Tooling
- `run.sh` compiles and runs the simulation:
  ```bash
  g++ -std=c++26 -O0 -Wall -Wextra -I ./include main.cpp
  ./a.out
  ```
- Uses C++26 mode and `std::print` / `std::println`.
- No external C++ dependencies beyond the standard library.
- Python dependencies: `numpy`, `pandas`, `matplotlib`, and `ffmpeg`.

## Project Structure

Typical layout:

```text
.
├── include/
│   ├── body.h
│   ├── constants.h
│   ├── forcefields.h
│   ├── integrators.h
│   ├── simulation.h
│   └── vector.h
├── main.cpp
├── vector_test.cpp
├── run.sh
├── visualization.py
└── build/
    ├── data.csv
    └── animation.mp4
```

## Getting Started

### Requirements

- C++ compiler with C++26/C++23 `std::print` support, such as GCC 14+.
- Python 3.8+
- Python packages:
  ```bash
  pip install numpy pandas matplotlib
  ```
- `ffmpeg` for MP4 export.

### Build and Run

```bash
mkdir -p build
./run.sh
```

This compiles `main.cpp`, runs the simulation, and writes:

```text
./build/data.csv
```

### Generate Animation

```bash
python3 visualization.py
```

This reads `./build/data.csv` and saves:

```text
./build/animation.mp4
```

## Extending

- Add a new force field by inheriting from `ForceField` and implementing `compute`.
- Add a new integrator by inheriting from `Integrator` and implementing `step`.
- Combine multiple force fields with `CompositeField`.
- Change physical constants in `include/constants.h`.
- Adjust simulation parameters in `main.cpp`.

## Constants

Important constants are defined in `include/constants.h`:

| Constant | Value | Purpose |
| --- | --- | --- |
| `BOX_SIZE` | `1.00` | Periodic box half-width |
| `G` | `6.67430e-3` | Gravitational constant |
| `R_SOFT_GRAV` | `1e-6` | Gravity softening |
| `R_CRIT_GRAV` | `5e-2` | Gravity critical radius |
| `EPSILON_LJ` | `1.00` | Lennard-Jones epsilon |
| `SIGMA_LJ` | `1.00` | Lennard-Jones sigma |
| `R_SOFT_LJ` | `0.7` | Lennard-Jones softening |
| `R_CRIT_LJ` | `0.0` | Lennard-Jones critical radius |
| `R_CUTOFF_LJ` | `2.5` | Lennard-Jones cutoff |

## License

No license is included in this repository. Add one before distributing.