#pragma once
#include<array>
#include<type_traits>
#include<cmath>
#include<algorithm>
#include<concepts>

template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
class Vector{
    private:
    std::array<T, Dim> coords;
    public:
    constexpr Vector(const std::array<T, Dim>&);
    constexpr const T& operator[](unsigned long) const;
    constexpr T& operator[](unsigned long);

    constexpr Vector operator+(const Vector&) const;
    constexpr Vector operator-(const Vector&) const;
    constexpr Vector operator*(T) const;
    constexpr Vector operator/(T) const;    
    constexpr void operator+=(const Vector&);
    constexpr void operator-=(const Vector&);
    constexpr void operator*=(T);
    constexpr void operator/=(T);
    constexpr T dot(const Vector&) const;
    constexpr bool equal(const Vector&, double) const;
    constexpr T norm() const;
    constexpr T norm1() const; 
    constexpr T normInf() const;
    constexpr Vector normalized() const;
};

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim> Vector<T, Dim>::normalized() const{
    return (*this)/norm();
}
template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr void Vector<T, Dim>::operator+=(const Vector& v){
    for (size_t i = 0; i < Dim; i++)
    {
        coords[i] += v[i];
    }
}
template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr void Vector<T, Dim>::operator-=(const Vector& v){
    for (size_t i = 0; i < Dim; i++)
    {
        coords[i] -= v[i];
    }
}
template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr void Vector<T, Dim>::operator*=(T a){
    for (size_t i = 0; i < Dim; i++)
    {
        coords[i] *= a;
    }
}
template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr void Vector<T, Dim>::operator/=(T a){
    if(a <= 0.00) throw std::invalid_argument("Division by Near-Zero!");
    for (size_t i = 0; i < Dim; i++)
    {
        coords[i] /= a;
    }
}
template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim>::Vector(const std::array<T, Dim>& c) : coords(c){}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim> Vector<T, Dim>::operator+(const Vector& v) const{
    std::array<T, Dim> curr;
    for (size_t i = 0; i < Dim; i++)
    {
        curr[i] = (coords[i] + v[i]);
    }
    return Vector{curr};    
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim> Vector<T, Dim>::operator-(const Vector& v) const{
    std::array<T, Dim> curr;
    for (size_t i = 0; i < Dim; i++)
    {
        curr[i] = (coords[i] - v[i]);
    }
    return Vector{curr}; 
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr T Vector<T, Dim>::dot(const Vector& v) const{
    T sum{};
    for (size_t i = 0; i < Dim; i++)
    {
        sum += coords[i] * v[i];
    }
    return sum;
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim> Vector<T, Dim>::operator*(T a) const{
    std::array<T, Dim> curr;
    for (size_t i = 0; i < Dim; i++)
    {
        curr[i] = (coords[i] * a);
    }
    return Vector{curr}; 
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr Vector<T, Dim> Vector<T, Dim>::operator/(T a) const{
    if(a <= 0.00) throw std::invalid_argument("Division by Near-Zero!");
    std::array<T, Dim> curr;
    for (size_t i = 0; i < Dim; i++)
    {
        curr[i] = (coords[i] / a);
    }
    return Vector{curr}; 
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr bool Vector<T, Dim>::equal(const Vector& v, double eps) const{
    for (size_t i = 0; i < Dim; i++)
    {
        if(std::abs(coords[i] - v[i]) > eps) return false;
    }
    return true;
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr T Vector<T, Dim>::norm() const{
    double sum{};
    for(const auto& c : coords){
        sum += c*c;
    }
    return static_cast<T>(std::sqrt(sum));
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr T Vector<T, Dim>::norm1() const{
    T sum{};
    for(const auto& c : coords){
        sum += static_cast<T>(std::abs(c));
    }
    return sum;
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr T Vector<T, Dim>::normInf() const{
    return *std::max_element(coords.begin(), coords.end(), [](const T& a, const T& b) -> bool{return std::abs(a) < std::abs(b);});
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr const T& Vector<T, Dim>::operator[](unsigned long i) const{
    return coords[i];
}

template<arithmetic T, unsigned long Dim> requires (Dim >= 1)
constexpr T& Vector<T, Dim>::operator[](unsigned long i){
    return coords[i];
}

using Vector2i = Vector<int, 2>;
using Vector2f = Vector<float, 2>;
using Vector2d = Vector<double, 2>;
using Vector3i = Vector<int, 3>;
using Vector3f = Vector<float, 3>;
using Vector3d = Vector<double, 3>;