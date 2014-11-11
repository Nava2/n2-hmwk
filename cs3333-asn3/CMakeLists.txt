cmake_minimum_required(VERSION 2.8)

project( CS3333-Assignment3 )

IF(NOT(CMAKE_BUILD_TYPE))
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

SET(CMAKE_CXX_FLAGS "-std=c++0x")

# find OpenCV
find_package( OpenCV REQUIRED )

add_executable( asn3 asn3.cpp)

target_link_libraries( asn3
    ${OpenCV_LIBS} )
