#pragma once
#include"forcefields.h"
#include<vector>
#include<stdexcept>
#include<memory>
#include<cmath>

inline double wrap(double x, double half_len) {
    const double len = 2.0 * half_len;
    x = std::fmod(x + half_len, len);
    if (x <= 0.0) x += len;
    return x - half_len;
}

class Integrator//interface
{
public:
    virtual void step(std::vector<Body>&, std::vector<Vector2d>&, const std::unique_ptr<ForceField>& ,double) const = 0;
    virtual ~Integrator() = default;
};


class EulerIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector2d>&, const std::unique_ptr<ForceField>&,  double) const override final;
};

void EulerIntegrator::step(std::vector<Body>& system, std::vector<Vector2d>& accs, const std::unique_ptr<ForceField>& ff, double dt) const{
    if (system.size() != accs.size()){throw std::invalid_argument("mismatch size of particles and accelerations");}
    const size_t n = system.size();
    if (n == 0 || dt == 0.0){return;}
    if (!ff){throw std::invalid_argument("force field is null");}

    for (size_t i = 0; i < n; i++)
    {  
        Body& particle {system[i]};
        const Vector2d& acc {accs[i]};
        particle.set_vel(particle.get_vel() + acc*dt);
        particle.set_pos(particle.get_pos() + particle.get_vel()*dt);
    }
    std::fill(accs.begin(), accs.end(), Vector2d({0.0, 0.0}));
    ff->compute(system, accs);
    
}

class RK4Integrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector2d>&, const std::unique_ptr<ForceField>&, double) const override final;
};
void RK4Integrator::step(std::vector<Body>& system, std::vector<Vector2d>& accs, const std::unique_ptr<ForceField>& ff, double dt) const
{
    if (system.size() != accs.size()){throw std::invalid_argument("mismatch size of particles and accelerations");}
    const size_t n = system.size();
    if (n == 0 || dt == 0.0){return;}
    if (!ff){throw std::invalid_argument("force field is null");}

    std::vector<Vector2d> x0(n), v0(n);

    for (size_t i = 0; i < n; ++i)
    {
        x0[i] = system[i].get_pos();
        v0[i] = system[i].get_vel();
    }

    std::vector<Body> tmp = system;

    auto evaluate = [&](const std::vector<Vector2d>& x, const std::vector<Vector2d>& v, std::vector<Vector2d>& a){
        for (size_t i = 0; i < n; ++i)
        {
            tmp[i] = Body(x[i], v[i], system[i].get_mass());
        }
        std::fill(a.begin(), a.end(), Vector2d({0.0, 0.0}));
        ff->compute(tmp, a);
    };
    std::vector<Vector2d> k1_dx(n), k1_dv(n);
    std::vector<Vector2d> k2_dx(n), k2_dv(n);
    std::vector<Vector2d> k3_dx(n), k3_dv(n);
    std::vector<Vector2d> k4_dx(n), k4_dv(n);

    std::vector<Vector2d> x(n), v(n);

    // k1
    for (size_t i = 0; i < n; ++i){k1_dx[i] = v0[i];}

    evaluate(x0, v0, k1_dv);

    const double half_dt = 0.5 * dt;

    // k2
    for (size_t i = 0; i < n; ++i){
        x[i] = x0[i] + k1_dx[i] * half_dt;
        v[i] = v0[i] + k1_dv[i] * half_dt;
    }

    evaluate(x, v, k2_dv);

    for (size_t i = 0; i < n; ++i){k2_dx[i] = v[i];}

    // k3
    for (size_t i = 0; i < n; ++i){
        x[i] = x0[i] + k2_dx[i] * half_dt;
        v[i] = v0[i] + k2_dv[i] * half_dt;
    }

    evaluate(x, v, k3_dv);

    for (size_t i = 0; i < n; ++i){k3_dx[i] = v[i];}

    // k4
    for (size_t i = 0; i < n; ++i){
        x[i] = x0[i] + k3_dx[i] * dt;
        v[i] = v0[i] + k3_dv[i] * dt;
    }

    evaluate(x, v, k4_dv);

    for (size_t i = 0; i < n; ++i){k4_dx[i] = v[i];}
    const double w = dt / 6.0;

    for (size_t i = 0; i < n; ++i)
    {
        Vector2d new_pos = x0[i] + (k1_dx[i] + k2_dx[i]*2.0 +k3_dx[i]*2.0 + k4_dx[i]) * w;
        Vector2d new_vel = v0[i] + (k1_dv[i] +k2_dv[i]*2.0 + k3_dv[i]*2.0 + k4_dv[i]) * w;
        new_pos[0] = (wrap(new_pos[0], BOX_SIZE));
        new_pos[1] = (wrap(new_pos[1], BOX_SIZE));
        system[i].set_pos(new_pos);
        system[i].set_vel(new_vel);
    }
    std::fill(accs.begin(), accs.end(), Vector2d({0.0, 0.0}));
    ff->compute(system, accs);
}

class VerletIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, std::vector<Vector2d>&, const std::unique_ptr<ForceField>&, double) const override final;
};
void VerletIntegrator::step(std::vector<Body>& system, std::vector<Vector2d>& accs, const std::unique_ptr<ForceField>& ff, double dt) const{
    if (system.size() != accs.size()){throw std::invalid_argument("mismatch size of particles and accelerations");}
    const size_t n = system.size();
    if (n == 0 || dt == 0.0){return;}
    if (!ff){throw std::invalid_argument("force field is null");}
    for (size_t i = 0; i < n; i++)
    {
        Body& particle {system.at(i)};
        const Vector2d& acc {accs.at(i)};
        
        particle.set_vel(particle.get_vel() + acc*(dt*0.5));

        Vector2d new_pos = particle.get_pos() + particle.get_vel()*dt;
        new_pos[0] = (wrap(new_pos[0], BOX_SIZE));
        new_pos[1] = (wrap(new_pos[1], BOX_SIZE));
        particle.set_pos(new_pos);
    }
    std::fill(accs.begin(), accs.end(), Vector2d({0.0, 0.0}));
    ff->compute(system, accs);    
    for (size_t i = 0; i < n; i++)
    {
        Body& particle {system.at(i)};
        const Vector2d& acc {accs.at(i)};  
        particle.set_vel(particle.get_vel() + acc*(dt*0.5));
    }
    
}

