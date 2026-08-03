#include"../include/body.h"
#include"../include/vector3d.h"
#include"../include/visualization.h"
#include"../include/integrators.h"
#include"../include/forcefields.h"
#include<vector>
#include<memory>

int main() {
    BodyVis vis(1200, 1000, 2.00f, "Two-Body Gravity Test");

    std::vector<Body> bodies;
    bodies.emplace_back(
        Vector3d(0.0, 0.0, 0.0),
        Vector3d(0.0, 0.0, 0.0),
        100
    );
    bodies.emplace_back(
        Vector3d(0.5, 0.0, 0.0),
        Vector3d(0.0, 0.8, 0.0),
        10
    );
    bodies.emplace_back(
        Vector3d(0.75, 0.0, 0.0),
        Vector3d(0.0, 0.2, 0.0),
        1
    );
    std::vector<Vector3d> accs;
    accs.emplace_back(0.0,0.0,0.0);
    accs.emplace_back(0.0,0.0,0.0);
    accs.emplace_back(0.0,0.0,0.0);

    std::unique_ptr<Integrator> euler(new EulerIntegrator);
    std::unique_ptr<ForceField> grav(new GravityForce);
    // Corrected: vector of std::array<float,3>
    std::vector<std::array<float,3>> colors = {
        {1.0f, 0.8f, 0.2f},  // yellow sun
        {0.3f, 0.6f, 1.0f},   // blue planet
        {1.0f, 0.6f, 1.0f}   // red planet
    };
    const double dt = 0.0005;
    while (vis.isOpen()) {
        if (!vis.processEvents()) break;

        // 10 substeps per frame
        for (int i = 0; i < 10; ++i)
            euler->step(bodies, accs, grav, dt);

        vis.clear(0.02f, 0.02f, 0.05f);
        vis.drawBodies(bodies, colors);
        vis.display();
    }

    return 0;
}