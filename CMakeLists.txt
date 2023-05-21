cmake_minimum_required(VERSION 3.10)

# project command populates multiple variables used in the building process.
project(2048 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Catch2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(2048 main.cpp utils.cpp math.cpp anim.cpp grid.cpp)
add_executable(test main_test.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(2048 SDL2 SDL2_image SDL2_mixer SDL2_ttf OpenGL)
