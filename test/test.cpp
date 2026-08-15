#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <print>
#include <cmath>
#include "../include/vector.h"

int main(){
    Vector2d v1{{1.0, 2.0}};
    Vector2d v2{{3.0, 4.0}};
    v2 += v1;
    v2 = v2.normalized();
    std::print("Res: {}, {}\n", v2[0], v2[1]);
    return 0;
}