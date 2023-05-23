cmake_minimum_required(VERSION 3.10)

# project command populates multiple variables used in the building process.
project(2048 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Catch2 REQUIRED)
find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)


add_executable(2048 main.cpp utils.cpp math.cpp anim.cpp grid.cpp state.cpp game.cpp)
add_executable(test main_test.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(2048 sfml-system sfml-window sfml-audio sfml-graphics)
