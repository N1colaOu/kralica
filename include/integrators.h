#pragma once
#include"body.h"
#include<vector>

class Integrator
{
public:
    virtual void step(std::vector<Body>&, double) = 0;
    virtual ~Integrator() = default;
};

class EulerIntegrator : public Integrator
{

};

class RK4Integrator : public EulerIntegrator
{

};

class VerletIntegrator : public Integrator
{

};
