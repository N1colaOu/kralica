#pragma once
#include<cmath>
#include<stdexcept>
#include<vector>
#include<algorithm>

class Vector3d
{
private:
    double x, y, z;
public:
    static constexpr double epsilon{1e-6};
    constexpr Vector3d(double _x=0.00, double _y=0.00, double _z=0.00);
    ~Vector3d() = default;
    Vector3d(const Vector3d&) = default; //copy ctor
    Vector3d(Vector3d&&) = default; //move ctor
    Vector3d& operator=(const Vector3d&) = default; //assign operator copy
    Vector3d& operator=(Vector3d&&) = default; //assign operator move
    constexpr double get_x() const;
    constexpr double get_y() const;
    constexpr double get_z() const;
    constexpr void set_x(double);
    constexpr void set_y(double);
    constexpr void set_z(double);

    constexpr Vector3d operator+(const Vector3d&) const;
    constexpr Vector3d operator-(const Vector3d&) const;
    constexpr double dot(const Vector3d&) const;
    constexpr Vector3d cross(const Vector3d&) const;
    constexpr Vector3d operator*(double) const;
    constexpr Vector3d operator/(double) const;
    inline bool operator==(const Vector3d&) const;
    constexpr double norm() const;
    constexpr double norm1() const; 
    constexpr double normInf() const; 
    constexpr double len_sqrd() const;
    constexpr Vector3d mod(double) const;
    constexpr Vector3d get_max_one() const; 
    constexpr double get_max_el() const; 
};

constexpr Vector3d::Vector3d(double _x, double  _y, double _z) : x{_x}, y{_y}, z{_z} {}
constexpr double Vector3d::get_x() const { return x; }
constexpr double Vector3d::get_y() const { return y; }
constexpr double Vector3d::get_z() const { return z; }
constexpr void Vector3d::set_x(double _x) {x=_x;}
constexpr void Vector3d::set_y(double _y) {y=_y;}
constexpr void Vector3d::set_z(double _z) {z=_z;}

constexpr Vector3d Vector3d::operator+(const Vector3d& v) const{
    return Vector3d{x+v.get_x(), y+v.get_y(), z+v.get_z()};
}
constexpr Vector3d Vector3d::operator-(const Vector3d& v) const{
    return Vector3d{x-v.get_x(), y-v.get_y(), z-v.get_z()};
}
constexpr double Vector3d::dot(const Vector3d& v) const{
    return x*v.get_x() + y*v.get_y() + z*v.get_z();
}
constexpr Vector3d Vector3d::cross(const Vector3d& v) const{
    return Vector3d{y*v.get_z() - z*v.get_y(),
                z*v.get_x() - x*v.get_z(),
                x*v.get_y() - y*v.get_x()};
}
constexpr Vector3d Vector3d::operator*(double a) const{
    return Vector3d{x*a, y*a, z*a};
}
constexpr Vector3d Vector3d::operator/(double a) const{
    if (std::abs(a) < epsilon)
        throw std::runtime_error("division by near-zero value");
    return {x/a, y/a, z/a};
}
inline bool Vector3d::operator==(const Vector3d& v) const{
    return std::abs(x-v.get_x()) <= epsilon && std::abs(y-v.get_y()) <= epsilon && std::abs(z-v.get_z()) <= epsilon;
}
constexpr double Vector3d::norm() const{
    return std::sqrt(x*x+y*y+z*z);
}
constexpr double Vector3d::len_sqrd() const{
    return x*x+y*y+z*z;
}
constexpr Vector3d Vector3d::mod(double a) const{
    return {std::fmod(x, a), std::fmod(y, a), std::fmod(z, a)};
}
constexpr double Vector3d::norm1() const{
    return std::abs(x) + std::abs(y) + std::abs(z);
}
constexpr double Vector3d::normInf() const{
    return std::max({std::abs(x), std::abs(y), std::abs(z)});
}
constexpr Vector3d Vector3d::get_max_one() const{
    Vector3d base{0,0,0};
    const double max_val = this->normInf();
    if(std::abs(x) >= max_val) base.set_x(1.00);
    if(std::abs(y) >= max_val) base.set_y(1.00);
    if(std::abs(z) >= max_val) base.set_z(1.00);
    return base;
}
constexpr double Vector3d::get_max_el() const{
    const double z_m = std::abs(z);
    const double x_m = std::abs(x);
    const double y_m = std::abs(y);
    if(z_m >= y_m && z_m >= x_m){
        return z;
    }
    else if (y_m >= x_m && y_m >= z_m){
        return y;
    }
    return x;
}