#include<print>
#include"../include/body.h"
#include"../include/integrators.h"
#include<memory>

int main(){
    Body p1{{0.0,0.0,0.0}, {0.0,0.0,0.0}, 1.0};
    Vector3d acc1({0.0,1.0,0.0});
    std::vector<Body> system{p1};
    std::vector<Vector3d> accs{acc1};
    std::unique_ptr<Integrator> euler(new EulerIntegrator);
    for (size_t i = 0; i < 10; i++)
    {
        euler->step(system, accs, 0.1);
        std::print("Position {}, {}, {}\n", system[0].get_pos().get_x(), system[0].get_pos().get_y(), system[0].get_pos().get_z());
    }
    return 0;
}