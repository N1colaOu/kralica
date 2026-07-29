#pragma once
#include"body.h"
#include<vector>

class Integrator//interface
{
public:
    virtual void step(std::vector<Body>&, double) const = 0;
    virtual ~Integrator() = default;
};


class EulerIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};

void EulerIntegrator::step(std::vector<Body>& system, double dt) const{
    for(auto& particle : system){
        const double m = particle.get_mass();
        //update force here if not constant
        const Vector3d acc = particle.get_force()/m;
        particle.set_speed(particle.get_speed() + acc*dt);
        particle.set_pos(particle.get_pos() + particle.get_speed()*dt);
    }
}

class RK4Integrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void RK4Integrator::step(std::vector<Body>& system, double dt) const{
    for(auto& particle : system){
        const double m = particle.get_mass();
        particle.set_speed(particle.get_speed() + particle.get_force()/m*dt);
        particle.set_pos(particle.get_pos() + particle.get_speed()*dt);
    }
}

class VerletIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void VerletIntegrator::step(std::vector<Body>& system, double dt) const{
    for(auto& particle : system){
        const double m = particle.get_mass();
        particle.set_pos();
    }
}
