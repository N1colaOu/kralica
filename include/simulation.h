#include "integrators.h"
#include<memory>
#include<vector>

class Simulation{
    private:
    std::vector<Body> system;
    std::vector<Vector2d> accs;
    std::unique_ptr<Integrator> integrator;
    CompositeField& force_fields;
    double dt;
    double current_time = 0.00;
    public:
    Simulation(const std::vector<Body>&,
            const std::vector<Vector2d>&, std::unique_ptr<Integrator>&, 
        const CompositeField&, double);
    void step();
    void run(double);
};
Simulation::Simulation(std::vector<Body>& b,
        std::vector<Vector2d>& a, std::unique_ptr<Integrator>& i, 
        CompositeField& ffs, double dt_) : 
        system(std::move(b)), accs(std::move(a)), integrator(std::move(i)), force_fields(std::move(ffs)), dt(dt_){}

void Simulation::step(){
    integrator->step(system, accs, force_fields, dt);
    current_time += dt;
}
void Simulation::run(double end_time){
    while(current_time <= end_time){
        step();
    }
}