#include <print>
#include <fstream>
#include <vector>
#include <memory>
#include <random>
#include <filesystem>
#include "simulation.h"

using namespace kralica;

int main() {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> num_dist(30, 80);
    const int num_particles = num_dist(gen);

    std::uniform_real_distribution<double> pos_dist(-BOX_SIZE, BOX_SIZE);

    std::vector<Body> system;
    std::vector<Vector2d> accs;

    system.reserve(num_particles);
    accs.reserve(num_particles);

    for (int i = 0; i < num_particles; ++i) {
        Vector2d pos({pos_dist(gen), pos_dist(gen)});
        Vector2d vel({0.0, 0.0});
        double mass = 1.0;
        system.emplace_back(pos, vel, mass);
        accs.emplace_back(Vector2d({0.0, 0.0}));
    }

    std::unique_ptr<Integrator> verlet(new VerletIntegrator);

    std::vector<std::unique_ptr<ForceField>> ff_vec;
    ff_vec.push_back(std::make_unique<LennardJonesForce>());
    CompositeField ff(std::move(ff_vec));

    const double dt = 1e-3;
    const double end_time = 2.0; 

    Simulation sim(system, accs, verlet, ff, dt);

    std::println("Running Lennard-Jones simulation with {} particles.", num_particles);
    sim.run_and_log(end_time, "./build/data.csv");

    return 0;
}