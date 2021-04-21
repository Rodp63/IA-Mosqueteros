cmake_minimum_required(VERSION 3.15)

project("Damas")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(damas src/main.cpp)

target_link_libraries(
  damas
  sfml-graphics
  sfml-window
  sfml-system
)
