#pragma once
#include"vector3d.h"
#include<stdexcept>

class Body
{
private:
    Vector3d pos;
    Vector3d speed;
    Vector3d force;
    double mass = 0.00;
public:
Body() = default;
constexpr Body(const Vector3d&, const Vector3d&, const Vector3d&, double);

Body(const Body&) = default; //copy ctor
Body(Body&&) = default; //move ctor
Body& operator=(const Body&) = default; //assign operator copy
Body& operator=(Body&&) = default; //assign operator move
~Body() = default; //dtor
};

constexpr Body::Body(const Vector3d& p, const Vector3d& s, const Vector3d& f, double m)
     : pos{p}, speed{s}, force{f}, mass{m} {
        if(mass <= 0.00) throw std::invalid_argument("mass must be positve");
}
