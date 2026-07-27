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

}

class RK4Integrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void RK4Integrator::step(std::vector<Body>& system, double dt) const{

}

class VerletIntegrator : public Integrator
{
public:
    void step(std::vector<Body>&, double) const override final;
};
void VerletIntegrator::step(std::vector<Body>& system, double dt) const{

}
