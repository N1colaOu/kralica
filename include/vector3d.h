#pragma once

class Vector3d
{
private:
    double x, y, z;
public:
    inline Vector3d(double x=0.00, double y=0.00, double z=0.00);
    inline ~Vector3d() = default;
    inline double get_x() const;
    inline double get_y() const;
    inline double get_z() const;
};

inline Vector3d::Vector3d(double _x, double  _y, double _z) : x(_x), y(_y), z(_z) {}
inline double Vector3d::get_x() const { return x; }
inline double Vector3d::get_y() const { return y; }
inline double Vector3d::get_z() const { return z; }