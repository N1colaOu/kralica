#include<print>
#include"../include/body.h"
#include"../include/integrators.h"
#include"../include/forcefields.h"
#include<memory>

int main(){
    Body p1{{1e20,0.0,0.0}, {0.0,0.0,0.0}, 1e20};
    Body p2{{0.0,0.0,0.0}, {0.0,0.0,0.0}, 1e10};
    Vector3d acc1({0.0,0.0,0.0});
    Vector3d acc2({0.0,0.0,0.0});
    std::vector<Body> system{p1, p2};
    std::vector<Vector3d> accs{acc1, acc2};
    std::unique_ptr<Integrator> euler(new EulerIntegrator);
    std::unique_ptr<ForceField> grav(new GravityForce);

    for (size_t i = 0; i < 20; i++)
    {
        euler->step(system, accs, grav, 0.2);
        std::print("Position1: {}, {}, {}\n", system[0].get_pos().get_x(), system[0].get_pos().get_y(), system[0].get_pos().get_z());
        std::print("Position2: {}, {}, {}\n", system[1].get_pos().get_x(), system[1].get_pos().get_y(), system[1].get_pos().get_z());
    }
    return 0;
}