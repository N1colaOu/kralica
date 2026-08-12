g++ -std=c++23 -O0 -I ./include -I ./glad -o ./sim test/test_lj.cpp src/visualization.cpp glad/glad.c -lglfw -ldl -lpthread
./sim