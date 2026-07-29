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
        //update force/acc here if not constant
        //const Vector3d acc = particle.get_acc()/m;
        particle.set_vel(particle.get_vel() + particle.get_acc()*dt);
        particle.set_pos(particle.get_pos() + particle.get_vel()*dt);
    }
}

class RK4Integrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void RK4Integrator::step(std::vector<Body>& system, double dt) const{
    // for(auto& particle : system){
    //     const double m = particle.get_mass();
    //     particle.set_vel(particle.get_vel() + particle.get_acc()/m*dt);
    //     particle.set_pos(particle.get_pos() + particle.get_vel()*dt);
    // }
}

class VerletIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void VerletIntegrator::step(std::vector<Body>& system, double dt) const{
    for(auto& particle : system){
        const double m = particle.get_mass();
        const Vector3d acc_old = particle.get_acc();
        particle.set_pos(particle.get_pos() + particle.get_vel()*dt + acc_old*(dt*dt*0.5));
        //update acc here 
        //const Vector3d new_acc = particle.get_acc()/m;
        particle.set_vel(particle.get_vel() + (particle.get_acc()+acc_old)*(dt*0.5));
    }
}
