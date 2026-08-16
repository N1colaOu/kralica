#pragma once
#include"vector.h"
#include<stdexcept>

using kralica::Vector2d;

class Body
{
private:
    Vector2d pos;
    Vector2d vel;
    double mass = 1.00;
public:
Body() = default;
constexpr Body(const Vector2d&, const Vector2d&, double);

constexpr const Vector2d& get_pos() const;
constexpr const Vector2d& get_vel() const;
constexpr double get_mass() const;

constexpr void set_pos(const Vector2d&);
constexpr void set_vel(const Vector2d&);
constexpr void set_mass(double);
};

constexpr Body::Body(const Vector2d& p, const Vector2d& s, double m)
     : pos{p}, vel{s}, mass{m} {
        if(mass <= 0.00) throw std::invalid_argument("mass must be positve");
}
   
constexpr const Vector2d& Body::get_pos() const { return pos; }
constexpr const Vector2d& Body::get_vel() const { return vel; }
constexpr double Body::get_mass() const { return mass; }

constexpr void Body::set_pos(const Vector2d& p) {pos = p;}
constexpr void Body::set_vel(const Vector2d& s) {vel = s;}
constexpr void Body::set_mass(double m) {mass = m;}