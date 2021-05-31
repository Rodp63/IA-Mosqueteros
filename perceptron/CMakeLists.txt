cmake_minimum_required(VERSION 3.19.1)

project("OCR Perceptron")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ocr_perceptron src/main.cpp)

target_link_libraries(
  ocr_perceptron
  sfml-graphics
  sfml-window
  sfml-system
)
