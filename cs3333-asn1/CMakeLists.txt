cmake_minimum_required(VERSION 2.8)

project( CS3333-Assignment1 )

SET(CMAKE_CXX_FLAGS "-std=c++0x")

# find OpenCV
find_package( OpenCV REQUIRED )

add_executable( asn1 asn1.cpp)

target_link_libraries( asn1
    ${OpenCV_LIBS} )
