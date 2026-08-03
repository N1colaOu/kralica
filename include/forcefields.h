#pragma once
#include<vector>
#include<stdexcept>
#include"integrators.h"
#include"constants.h"
#include"vector3d.h"
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
            const double rad_soft = 1/std::sqrt(rad_ij.len_sqrd() + GRAV_SOFT*GRAV_SOFT);
            const double coeff = G*rad_soft*rad_soft*rad_soft;
            const Vector3d grav_force_ij{rad_ij*coeff};

            accs[j] = accs[j] - grav_force_ij*m_i;
            accs[i] = accs[i] + grav_force_ij*m_j;
        }
    }
}