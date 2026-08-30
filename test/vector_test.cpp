#include"../include/vector.h"
#include<cmath>
#include<stdexcept>
#include<iostream>

using namespace kralica;

void test_vectors(){
    constexpr Vector2d v1{{1.00, 2.00}};
    constexpr Vector2d v2{{2.00, -1.00}};
    constexpr Vector2d v_sum{{3.00, 1.00}};
    constexpr Vector2d v_sub{{-1.00, 3.00}};
    constexpr Vector2d v_mul{{2.00, 4.00}};
    constexpr Vector2d v_div{{1.00, -0.500}};
    static_assert(v1 + v2 == v_sum, "Vector Sumation Error!");
    static_assert(v1 - v2 == v_sub, "Vector Subtraction Error!");
    static_assert(v2 / 2.00 == v_div, "Vector Division Error!");
    static_assert(v1 * 2.00 == v_mul, "Vector Multiplication Error!");
    static_assert(v1.dot(v2) == 0.00, "Vector Subtraction Error!");
    static_assert(v1.norm1() == 3.00, "Vector Norm 1 Error!");
    static_assert(v1.norm() == std::sqrt(5), "Vector Norm 2 Error!");
    static_assert(v1.normInf() == 2.00, "Vector Norm Inf Error!");

    constexpr Vector2d zero{{0.00, 0.00}};
    constexpr Vector2d rel_zero{{1e-9, 1e-9}};
    static_assert(zero.equal(rel_zero, 1e-9), "Vector Approximate Equality Error!");

    constexpr Vector3d vc1{{1.00, 2.00, 3.00}};
    constexpr Vector3d vc2{{1.00, -2.00, 3.00}};
    constexpr Vector3d vc3{{12.00, 0.00, -4.00}};
    static_assert(vc1.cross(vc2) == vc3);

}