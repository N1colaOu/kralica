#include "../include/visualization.h"
#include "../include/integrators.h"
#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <print>
#include <cmath>

int main() {
    // Simulation parameters
    const size_t N = 400;        //particles
    const double v0 = 0.2;                   // typical velocity scale (root‑mean‑square)
    const double dt = 1e-4;               // time step

    // Lennard‑Jones constants (reduced units, soft core for stability)
    // These constants are expected to be defined globally or in LennardJonesForce.
    // If they are member variables, set them via the constructor or setters.
    // For demonstration we assume they are defined as global constants.
    // In practice you should set them in the LennardJonesForce object.
    // const double SIGMA_LJ    = 1.0;
    // const double EPSILON_LJ  = 1.0;
    // const double R_SOFT_LJ   = 1.0;   // large soft core prevents force explosion
    // const double R_CRIT_LJ   = 0.0;
    // const double R_CUTOFF_LJ = 2.5;

    // Random number generator
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_real_distribution<double> posDist(-BOX_SIZE, BOX_SIZE); 
    std::normal_distribution<double> velDist(0.0, v0 / std::sqrt(3.0)); // Maxwell–Boltzmann component

    // Create bodies with random positions and velocities
    std::vector<Body> bodies;
    bodies.reserve(N);
    for (size_t i = 0; i < N; ++i) {
        Vector3d pos(posDist(rng), posDist(rng), 0);
        Vector3d vel(velDist(rng), velDist(rng), 0);
        double mass = 1.0;   // all masses equal
        bodies.emplace_back(pos, vel, mass);
    }

    // Accelerations array (size N, zero initially)
    std::vector<Vector3d> accs(N, Vector3d(0.0, 0.0, 0.0));

    // Integrator and force field (ensure LJ constants are set appropriately!)
    std::unique_ptr<Integrator> verlet(new VerletIntegrator);
    std::unique_ptr<ForceField> lj(new LennardJonesForce);
    // Example: if LennardJonesForce has a method to set parameters, call it here.
    // lj->setParameters(1.0, 1.0, 1.0, 0.0, 2.5);

    // Random colors for visualization
    std::vector<std::array<float, 3>> colors(N);
    std::uniform_real_distribution<float> colDist(0.3f, 1.0f);
    for (auto& c : colors) {
        c = {colDist(rng), colDist(rng), colDist(rng)};
    }

    // Visualization setup
    // Adjust the view scaling to accommodate the box
    BodyVis vis(BOX_SIZE*1000, BOX_SIZE*1000, static_cast<float>(BOX_SIZE)*2.0f, "Lennard-Jones Random Test");

    while (vis.isOpen()) {
        if (!vis.processEvents()) break;

        vis.clear(0.02f, 0.02f, 0.05f);
        vis.drawBodies(bodies, colors);
        vis.display();

        // Perform one integration step
        verlet->step(bodies, accs, lj, dt);

        // Optional: print first particle's acceleration for debugging
        // std::print("a[0]: {:.3f}, {:.3f}, {:.3f}\n", accs[0].get_x(), accs[0].get_y(), accs[0].get_z());
    }

    return 0;
}