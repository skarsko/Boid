cmake_minimum_required (VERSION 3.7)
project (particleSimulationBoids)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})


add_executable (run main.cpp src/boid.cpp src/generateBoids.cpp)
target_link_libraries(run ${SDL2_LIBRARIES})