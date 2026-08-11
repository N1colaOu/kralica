#pragma once
#include<vector>
#include<stdexcept>
#include"constants.h"
#include"body.h"
#include<algorithm>

class ForceField
{
public:
virtual void compute(const std::vector<Body>&, std::vector<Vector3d>&) const = 0;
virtual ~ForceField() = default;
};

class GravityForce : public ForceField
{
public:
void compute(const std::vector<Body>&, std::vector<Vector3d>&) const override;
};

void GravityForce::compute(const std::vector<Body>& system, std::vector<Vector3d>& accs) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    std::fill(accs.begin(), accs.end(), Vector3d{0.00,0.00,0.00});
    for (size_t i = 0; i < n; i++)
    {   
        const Body& particle_i = system[i];
        const double m_i = particle_i.get_mass();
        const Vector3d pos_i = particle_i.get_pos();

        for (size_t j = i + 1; j < n; j++)
        {
            const Body& particle_j = system[j];
            const double m_j = particle_j.get_mass();
            const Vector3d pos_j = particle_j.get_pos();
            const Vector3d rad_ij = (pos_j-pos_i);
            const double rad_soft = 1/std::sqrt(rad_ij.len_sqrd() + R_SOFT_GRAV*R_SOFT_GRAV);
            const double coeff = G*rad_soft*rad_soft*rad_soft;
            const Vector3d grav_force_ij{rad_ij*coeff};

            accs[j] = accs[j] - grav_force_ij*m_i;
            accs[i] = accs[i] + grav_force_ij*m_j;
        }
    }
}

class LennardJonesForce : public ForceField
{
public:
void compute(const std::vector<Body>&, std::vector<Vector3d>&) const override;
};

void LennardJonesForce::compute(const std::vector<Body>& system, std::vector<Vector3d>& accs) const{
    if(system.size() != accs.size()) throw std::invalid_argument("mismatch size of particles and accelerations");
    size_t n = system.size();
    std::fill(accs.begin(), accs.end(), Vector3d{0.00,0.00,0.00});
    for (size_t i = 0; i < n; i++)
    {   
        const Body& particle_i = system[i];
        const double m_i = particle_i.get_mass();
        const Vector3d pos_i = particle_i.get_pos();

        for (size_t j = i + 1; j < n; j++)
        {
            const Body& particle_j = system[j];
            const double m_j = particle_j.get_mass();
            const Vector3d pos_j = particle_j.get_pos();
            const Vector3d rad_ij = (pos_j-pos_i);
            const double rad_norm = rad_ij.norm();
            if (rad_norm <= R_CRIT_LJ || rad_norm >= R_CUTOFF_LJ) continue;
            const double rad_soft = 1/(rad_norm + R_SOFT_LJ);
            const Vector3d r_hat = rad_ij/rad_norm;
            double x = SIGMA_LJ * rad_soft;
            double x2 = x * x;
            double x6 = x2 * x2 * x2;
            double x12 = x6 * x6;
            double force_mag = 24.0 * EPSILON_LJ * rad_soft * (2.0 * x12 - x6);
            const Vector3d lj_force_ij = r_hat*force_mag;
            accs[j] = accs[j] - lj_force_ij/m_j;
            accs[i] = accs[i] + lj_force_ij/m_i;
        }
    }
}