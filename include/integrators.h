#pragma once
#include"forcefields.h"
#include<vector>
#include<stdexcept>
#include<memory>
#include<cmath>

using ff_ptr = std::unique_ptr<ForceField>;

class Integrator//interface
{
public:
    virtual void step(std::vector<Body>&, std::vector<Vector3d>&, const ff_ptr& ,double) const = 0;
    virtual ~Integrator() = default;
};


class EulerIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector3d>&, const ff_ptr&,  double) const override final;
};

void EulerIntegrator::step(std::vector<Body>& system, std::vector<Vector3d>& accs, const ff_ptr& ff, double dt) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    ff->compute(system, accs);
    for (size_t i = 0; i < n; i++)
    {  
        Body& particle {system[i]};
        const Vector3d& acc {accs[i]};
        const double m = particle.get_mass();
        particle.set_vel(particle.get_vel() + acc*dt);
        particle.set_pos(particle.get_pos() + particle.get_vel()*dt);
    }
    
}

class RK4Integrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector3d>&, const ff_ptr&, double) const override final;
};
void RK4Integrator::step(std::vector<Body>& system, std::vector<Vector3d>& accs, const ff_ptr& ff, double dt) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    //v_new = v_old + 1/6*dt*(a1+2a2+2a3+a4)
    //x_new = x_old + 1/6*dt*(v1+2v2+2v3+v4)
}

class VerletIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector3d>&, const ff_ptr&, double) const override final;
};
void VerletIntegrator::step(std::vector<Body>& system, std::vector<Vector3d>& accs, const ff_ptr& ff, double dt) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    const std::vector<Vector3d> accs_old{accs};
    ff->compute(system, accs);    
    for (size_t i = 0; i < n; i++)
    {
        Body& particle {system.at(i)};
        const Vector3d& acc {accs.at(i)};
        const Vector3d& acc_old {accs_old.at(i)};
        const double m = particle.get_mass();
        const double box_check = particle.get_pos().normInf();
        
        particle.set_vel(particle.get_vel() + (acc+acc_old)*(dt*0.5));
        if(box_check < BOX_SIZE){
            particle.set_pos(particle.get_pos() + particle.get_vel()*dt + acc_old*(dt*dt*0.5));
        }
        else{
            if(particle.get_pos().get_max_el() < 0.00){
                particle.set_pos((particle.get_pos() + particle.get_vel()*dt + acc_old*(dt*dt*0.5)).mod(BOX_SIZE) 
                + particle.get_pos().get_max_one()*BOX_SIZE);
            }
            else{
                particle.set_pos((particle.get_pos() + particle.get_vel()*dt + acc_old*(dt*dt*0.5)).mod(BOX_SIZE) 
                - particle.get_pos().get_max_one()*BOX_SIZE);        
            }
        }
    }
}
