#include<print>
#include"../include/simulation.h"
#include<fstream>
#include<vector>
#include<memory>

using namespace kralica;

int main(){
    // Vector2d test{{1.00, 2.00}};
    // std::ofstream out("./build/data.csv", std::ios::out | std::ios::trunc);
    // std::println(out, "Test: {}\n", test);

    std::vector<Body> system{};
    system.emplace_back(Body(Vector2d({0.0, 0.0}), Vector2d({0.0, 0.0}), 500));
    system.emplace_back(Body(Vector2d({0.5, 0.0}), Vector2d({0.0, 1.0}), 1));
    system.emplace_back(Body(Vector2d({-0.5, 0.0}), Vector2d({0.0, -1.0}), 1));
    system.emplace_back(Body(Vector2d({0.0, 0.5}), Vector2d({-1.0, 0.0}), 1));
    system.emplace_back(Body(Vector2d({0.0, -0.5}), Vector2d({1.0, 0.0}), 1));

    std::vector<Vector2d> accs{};
    accs.emplace_back(Vector2d({0.00, 0.00}));
    accs.emplace_back(Vector2d({0.00, 0.00}));
    accs.emplace_back(Vector2d({0.00, 0.00}));
    accs.emplace_back(Vector2d({0.00, 0.00}));
    accs.emplace_back(Vector2d({0.00, 0.00}));

    std::unique_ptr<Integrator> verlet(new VerletIntegrator);

    std::vector<std::unique_ptr<ForceField>> ff_vec;
    ff_vec.push_back(std::make_unique<GravityForce>());
    CompositeField ff(std::move(ff_vec));

    const double dt = 1e-3;
    Simulation sim(system, accs, verlet, ff, dt);
    
    const double end_time = 2.00;
    sim.run_and_log(end_time, "./build/data.csv");


    return 0;
}