#pragma once
#include"forcefields.h"
#include<vector>
#include<stdexcept>
#include<memory>
#include<cmath>

using ff_ptr = std::unique_ptr<ForceField>;

inline double wrap(double x, double half_len) {
    const double len = 2.0 * half_len;
    x = std::fmod(x + half_len, len);
    if (x <= 0.0) x += len;
    return x - half_len;
}

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
    for (size_t i = 0; i < n; i++)
    {
        Body& particle {system.at(i)};
        const Vector3d& acc {accs.at(i)};
        const double m = particle.get_mass();
        
        particle.set_vel(particle.get_vel() + acc*(dt*0.5));

        Vector3d new_pos = particle.get_pos() + particle.get_vel()*dt + acc*(dt*dt*0.5);
        new_pos.set_x(wrap(new_pos.get_x(), BOX_SIZE));
        new_pos.set_y(wrap(new_pos.get_y(), BOX_SIZE));
        new_pos.set_z(wrap(new_pos.get_z(), BOX_SIZE));
        particle.set_pos(new_pos);
    }
    ff->compute(system, accs);    
    for (size_t i = 0; i < n; i++)
    {
        Body& particle {system.at(i)};
        const Vector3d& acc {accs.at(i)};  
        particle.set_vel(particle.get_vel() + acc*(dt*0.5));
    }
    
}

