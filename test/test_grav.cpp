#include"../include/visualization.h"
#include"../include/integrators.h"
#include<vector>
#include<memory>

using kralica::Vector2d;

int main() {
    BodyVis vis(1200, 1000, 2.00f, "Two-Body Gravity Test");

    std::vector<Body> bodies;
    bodies.emplace_back(
        Vector2d({0.0, 0.0}),
        Vector2d({0.0, 0.0}),
        100
    );
    bodies.emplace_back(
        Vector2d({0.3, 0.0}),
        Vector2d({0.0, 1.5}),
        1
    );
    bodies.emplace_back(
        Vector2d({0.0, 0.3}),
        Vector2d({-1.5, 0.0}),
        1
    );
    bodies.emplace_back(
        Vector2d({-0.5, 0.0}),
        Vector2d({0.0, -1.5}),
        1
    );
    std::vector<Vector2d> accs;
    accs.push_back({{0.0,0.0}});
    accs.push_back({{0.0,0.0}});
    accs.push_back({{0.0,0.0}});
    accs.push_back({{0.0,0.0}});

    std::unique_ptr<Integrator> verlet(new VerletIntegrator);
    std::unique_ptr<ForceField> grav(new GravityForce);
    // Corrected: vector of std::array<float,3>
    std::vector<std::array<float,3>> colors = {
        {1.0f, 0.8f, 0.2f},  // yellow sun
        {0.3f, 0.6f, 1.0f},  // yellow sun
        {0.3f, 0.6f, 0.0f}//,   // blue planet
        //{1.0f, 0.6f, 1.0f}   // red planet
    };
    const double dt = 0.005;
    while (vis.isOpen()) {
        if (!vis.processEvents()) break;

        verlet->step(bodies, accs, grav, dt);
        vis.clear(0.02f, 0.02f, 0.05f);
        vis.drawBodies(bodies, colors);
        vis.display();
    }

    return 0;
}