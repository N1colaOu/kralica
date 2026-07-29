#pragma once
#include"vector3d.h"
#include<stdexcept>

class Body
{
private:
    Vector3d pos;
    Vector3d vel;
    Vector3d acc;
    double mass = 1.00;
public:
Body() = default;
constexpr Body(const Vector3d&, const Vector3d&, const Vector3d&, double);

Body(const Body&) = default; //copy ctor
Body(Body&&) = default; //move ctor
Body& operator=(const Body&) = default; //assign operator copy
Body& operator=(Body&&) = default; //assign operator move
~Body() = default; //dtor

constexpr const Vector3d& get_pos() const;
constexpr const Vector3d& get_vel() const;
constexpr const Vector3d& get_acc() const;
constexpr double get_mass() const;

constexpr void set_pos(const Vector3d&);
constexpr void set_vel(const Vector3d&);
constexpr void set_acc(const Vector3d&);
constexpr void set_mass(double);
};

constexpr Body::Body(const Vector3d& p, const Vector3d& s, const Vector3d& f, double m)
     : pos{p}, vel{s}, acc{f}, mass{m} {
        if(mass <= 0.00) throw std::invalid_argument("mass must be positve");
}
   
constexpr const Vector3d& Body::get_pos() const { return pos; }
constexpr const Vector3d& Body::get_vel() const { return vel; }
constexpr const Vector3d& Body::get_acc() const { return acc; }
constexpr double Body::get_mass() const { return mass; }

constexpr void Body::set_pos(const Vector3d& p) {pos = p;}
constexpr void Body::set_vel(const Vector3d& s) {vel = s;}
constexpr void Body::set_acc(const Vector3d& f) {acc = f;}
constexpr void Body::set_mass(double m) {mass = m;}