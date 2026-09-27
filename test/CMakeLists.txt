
add_library(test OBJECT
vector_test.cpp
forcefield_test.cpp
integrator_test.cpp)

target_link_libraries(test PRIVATE libs)