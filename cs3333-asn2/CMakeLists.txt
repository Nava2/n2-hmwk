cmake_minimum_required(VERSION 2.8)

project( CS3333-Assignment2 )

IF(NOT(CMAKE_BUILD_TYPE))
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

SET(CMAKE_CXX_FLAGS "-std=c++0x")

# find OpenCV
find_package( OpenCV REQUIRED )

add_executable( asn2 asn2.cpp)

target_link_libraries( asn2
    ${OpenCV_LIBS} )
