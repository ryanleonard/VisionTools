cmake_minimum_required(VERSION 3.17)
project(sgm)

set(CMAKE_CXX_STANDARD 14)

add_executable(sgm main.cpp)
find_package(OpenCV REQUIRED)
target_link_libraries(sgm ${OpenCV_LIBS})