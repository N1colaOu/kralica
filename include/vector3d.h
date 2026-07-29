#pragma once
#include<cmath>
#include <stdexcept>

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

    constexpr Vector3d operator+(const Vector3d&) const;
    constexpr Vector3d operator-(const Vector3d&) const;
    constexpr double dot(const Vector3d&) const;
    constexpr Vector3d cross(const Vector3d&) const;
    constexpr Vector3d operator*(double) const;
    constexpr Vector3d operator/(double) const;
    inline bool operator==(const Vector3d&) const;

};

constexpr Vector3d::Vector3d(double _x, double  _y, double _z) : x{_x}, y{_y}, z{_z} {}
constexpr double Vector3d::get_x() const { return x; }
constexpr double Vector3d::get_y() const { return y; }
constexpr double Vector3d::get_z() const { return z; }

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


