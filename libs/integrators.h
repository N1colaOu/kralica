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

namespace kralica{

    class Integrator
    {
    public:
        virtual void step(std::vector<Body>&, std::vector<Vector2d>&, const CompositeField& ,double) const = 0;
        virtual ~Integrator() = default;
    };
    
    
    class EulerIntegrator : public Integrator
    {
    public:
        void step(std::vector<Body>&, std::vector<Vector2d>&, const CompositeField&,  double) const override final;
    };
    
    void EulerIntegrator::step(std::vector<Body>& system, std::vector<Vector2d>& accs, const CompositeField& ff, double dt) const{
        if (system.size() != accs.size()){throw std::invalid_argument("mismatch size of particles and accelerations");}
        const size_t n = system.size();
        if (n == 0 || dt == 0.0){return;}
    
    
        for (size_t i = 0; i < n; i++)
        {  
            Body& particle {system[i]};
            const Vector2d& acc {accs[i]};
            particle.set_vel(particle.get_vel() + acc*dt);
            particle.set_pos(particle.get_pos() + particle.get_vel()*dt);
        }
        std::fill(accs.begin(), accs.end(), Vector2d({0.0, 0.0}));
        ff.compute(system, accs);
        
    }
    class VerletIntegrator : public Integrator
    {
    public:
        void step(std::vector<Body>&, std::vector<Vector2d>&, const CompositeField&, double) const override final;
    };
    void VerletIntegrator::step(std::vector<Body>& system, std::vector<Vector2d>& accs, const CompositeField& ff, double dt) const{
        if (system.size() != accs.size()){throw std::invalid_argument("mismatch size of particles and accelerations");}
        const size_t n = system.size();
        if (n == 0 || dt == 0.0){return;}
    
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
        ff.compute(system, accs);    
        for (size_t i = 0; i < n; i++)
        {
            Body& particle {system.at(i)};
            const Vector2d& acc {accs.at(i)};  
            particle.set_vel(particle.get_vel() + acc*(dt*0.5));
        }
        
    }

}    
