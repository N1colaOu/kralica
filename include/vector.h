#pragma once
#include<array>
#include<cstddef>
#include<cmath>
#include<algorithm>
#include<concepts>
#include<stdexcept>


template <typename T>
concept arithmetic = std::signed_integral<T> || std::floating_point<T>;

namespace kralica{
    
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    class Vector{
        private:
        std::array<T, Dim> coords;
        public:
        constexpr Vector();
        constexpr Vector(const std::array<T, Dim>&);
        constexpr const T& operator[](std::size_t) const;
        constexpr T& operator[](std::size_t);
        
        constexpr Vector operator+(const Vector&) const;
        constexpr Vector operator-(const Vector&) const;
        constexpr Vector operator*(T) const;
        constexpr Vector operator/(T) const;    
        constexpr Vector& operator+=(const Vector&);
        constexpr Vector& operator-=(const Vector&);
        constexpr Vector& operator*=(T);
        constexpr Vector& operator/=(T);
        constexpr bool operator==(const Vector&) const;
        constexpr T dot(const Vector&) const;
        constexpr bool equal(const Vector&, double) const;
        constexpr T norm() const;
        constexpr T norm1() const; 
        constexpr T normInf() const;
        constexpr Vector normalized() const;
        constexpr Vector<T, 3> cross(const Vector<T, 3>&) const;
    };
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>::Vector() : coords{} {}

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr bool Vector<T, Dim>::operator==(const Vector<T, Dim>& v) const{
        for (size_t i = 0; i < Dim; i++)
        {
            if(coords[i] != v[i]) return false;
        }
        return true;
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, 3> Vector<T, Dim>::cross(const Vector<T, 3>& v) const{
        return Vector<T, 3>{{coords[1]*v[2] - coords[2]*v[1],coords[2]*v[0] - coords[0]*v[2],coords[0]*v[1] - coords[1]*v[0]}};
    }
    
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim> Vector<T, Dim>::normalized() const{
        return (*this)/norm();
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>& Vector<T, Dim>::operator+=(const Vector& v){
        for (size_t i = 0; i < Dim; i++)
        {
            coords[i] += v[i];
        }
        return *this;
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>& Vector<T, Dim>::operator-=(const Vector& v){
        for (size_t i = 0; i < Dim; i++)
        {
            coords[i] -= v[i];
        }
        return *this;
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>& Vector<T, Dim>::operator*=(T a){
        for (size_t i = 0; i < Dim; i++)
        {
            coords[i] *= a;
        }
        return *this;
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>& Vector<T, Dim>::operator/=(T a){
        if(a == T{0}) throw std::invalid_argument("Division by Zero!");
        for (size_t i = 0; i < Dim; i++)
        {
            coords[i] /= a;
        }
        return *this;
    }
    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim>::Vector(const std::array<T, Dim>& c) : coords(c){}

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim> Vector<T, Dim>::operator+(const Vector& v) const{
        Vector result = *this;
        result += v;
        return result;    
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim> Vector<T, Dim>::operator-(const Vector& v) const{
        Vector result = *this;
        result -= v;
        return result;
    }


    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim> Vector<T, Dim>::operator*(T a) const{
        Vector result = *this;
        result *= a;
        return result;
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr Vector<T, Dim> Vector<T, Dim>::operator/(T a) const{
        Vector result = *this;
        result /= a;
        return result;
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr T Vector<T, Dim>::dot(const Vector& v) const{
        T sum{};
        for (size_t i = 0; i < Dim; i++)
        {
            sum += coords[i] * v[i];
        }
        return sum;
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr bool Vector<T, Dim>::equal(const Vector& v, double eps) const{
        for (size_t i = 0; i < Dim; i++)
        {
            if(std::abs(coords[i] - v[i]) > eps) return false;
        }
        return true;
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr T Vector<T, Dim>::norm() const{
        double sum{};
        for(const auto& c : coords){
            sum += c*c;
        }
        return static_cast<T>(std::sqrt(sum));
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr T Vector<T, Dim>::norm1() const{
        T sum{};
        for(const auto& c : coords){
            sum += static_cast<T>(std::abs(c));
        }
        return sum;
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr T Vector<T, Dim>::normInf() const{
        return static_cast<T>(std::abs(*std::max_element(coords.begin(), coords.end(), [](const T& a, const T& b) -> bool{return std::abs(a) < std::abs(b);})));
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr const T& Vector<T, Dim>::operator[](std::size_t i) const{
        return coords[i];
    }

    template<arithmetic T, std::size_t Dim> requires (Dim >= 1)
    constexpr T& Vector<T, Dim>::operator[](std::size_t i){
        return coords[i];
    }

    using Vector2i = Vector<int, 2>;
    using Vector2f = Vector<float, 2>;
    using Vector2d = Vector<double, 2>;
    using Vector3i = Vector<int, 3>;
    using Vector3f = Vector<float, 3>;
    using Vector3d = Vector<double, 3>;
}