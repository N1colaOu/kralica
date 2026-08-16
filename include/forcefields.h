#pragma once
#include<vector>
#include<stdexcept>
#include"constants.h"
#include"body.h"
#include<algorithm>

using kralica::Vector2d;

class ForceField
{
public:
virtual void compute(const std::vector<Body>&, std::vector<Vector2d>&) const = 0;
virtual ~ForceField() = default;
};

class GravityForce : public ForceField
{
public:
void compute(const std::vector<Body>&, std::vector<Vector2d>&) const override;
};

void GravityForce::compute(const std::vector<Body>& system, std::vector<Vector2d>& accs) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    std::fill(accs.begin(), accs.end(), Vector2d{{0.00,0.00}});
    for (size_t i = 0; i < n; i++)
    {   
        const Body& particle_i = system[i];
        const double m_i = particle_i.get_mass();
        const Vector2d pos_i = particle_i.get_pos();

        for (size_t j = i + 1; j < n; j++)
        {
            const Body& particle_j = system[j];
            const double m_j = particle_j.get_mass();
            const Vector2d pos_j = particle_j.get_pos();
            const Vector2d rad_ij = (pos_j-pos_i);
            const double rad_norm = rad_ij.norm();
            const double rad_soft = 1/std::sqrt(rad_norm*rad_norm + R_SOFT_GRAV*R_SOFT_GRAV);
            const double force_mag = G*rad_soft*rad_soft*rad_soft;
            const Vector2d grav_force_ij = rad_ij*force_mag;

            accs[j] -= grav_force_ij*m_i;
            accs[i] += grav_force_ij*m_j;
        }
    }
}

class LennardJonesForce : public ForceField
{
public:
void compute(const std::vector<Body>&, std::vector<Vector2d>&) const override;
};

void LennardJonesForce::compute(const std::vector<Body>& system, std::vector<Vector2d>& accs) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    std::fill(accs.begin(), accs.end(), Vector2d{{0.00,0.00}});
    for (size_t i = 0; i < n; i++)
    {   
        const Body& particle_i = system[i];
        const double m_i = particle_i.get_mass();
        const Vector2d pos_i = particle_i.get_pos();

        for (size_t j = i + 1; j < n; j++)
        {
            const Body& particle_j = system[j];
            const Vector2d pos_j = particle_j.get_pos();
            const Vector2d rad_ij = (pos_j-pos_i);
            const double rad_norm = rad_ij.norm();
            if (rad_norm >= R_CUTOFF_LJ) continue;
            const double m_j = particle_j.get_mass();
            const double x = SIGMA_LJ * SIGMA_LJ / (rad_norm * rad_norm + R_SOFT_LJ * R_SOFT_LJ);
            double x3 = x * x * x;
            double x6 = x3 * x3;
            double force_mag = 24.0 * EPSILON_LJ * x / (SIGMA_LJ * SIGMA_LJ) * (2.0 * x6 - x3);
            const Vector2d lj_force_ij = rad_ij*force_mag;
            accs[j] += lj_force_ij/m_j;
            accs[i] -= lj_force_ij/m_i;
        }
    }
}