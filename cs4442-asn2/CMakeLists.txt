project(cs4442-asn2)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug")
    message("Setting to be debug")
endif()

SET( KBRIGHT2_ASN2_MAIN "true" )

# for C++11
IF(MINGW)
    set(CMAKE_CXX_FLAGS "-std=gnu++0x ${CMAKE_CXX_FLAGS}")
ELSEIF(UNIX)
    set( CMAKE_CXX_FLAGS "-std=gnu++11 -fPIC -Wall ${CMAKE_CXX_FLAGS}")
    IF (APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        message(STATUS "Apple Machine detected, Using libc++")
    ENDIF()
ENDIF()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

INCLUDE_DIRECTORIES("include/")

SET( PROV_HEADERS
    "include/VectorHash.h"
    "include/fileRead.h"
    "include/utilsToStudents.h"
)

SET( PROV_SRCS
    "src/fileRead.cpp"
)

# Test code
# add_executable("${PROJECT_NAME}-test" "test.cpp" ${PROV_HEADERS} ${PROV_SRCS})

SET( LIB_HEADERS
    "include/kbright2/database.h"
    "include/kbright2/utils.h"
    "include/kbright2/smoothing/good_turing.h"
    "include/kbright2/smoothing/add_delta.h"
)

SET( LIB_SRCS

)

SET( P1_HEADERS
    # "include/kbright2/p1/..."
)

SET( P1_SRCS
    "src/kbright2/p1/p1.cpp"
)

add_executable("P1" ${P1_HEADERS} ${P1_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})
    
SET( P1_FIND_LONGEST_SRCS 
    "src/kbright2/p1/p1-find-longest.cpp"
)

add_executable("P1-find-longest" ${P1_HEADERS} ${P1_FIND_LONGEST_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})

SET( P2_HEADERS
    # "include/kbright2/p1/..."
)

SET( P2_SRCS
    "src/kbright2/p2/p2.cpp"
)

add_executable("P2" ${P2_HEADERS} ${P2_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})
    
    SET( P2_HEADERS
    # "include/kbright2/p1/..."
)

SET( P3_HEADERS
    # "include/kbright2/p1/..."
)

SET( P3_SRCS
    "src/kbright2/p3/p3.cpp"
)

add_executable("P3" ${P3_HEADERS} ${P3_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})

SET( P4_HEADERS
    # "include/kbright2/p1/..."
)

SET( P4_SRCS
    "src/kbright2/p4/p4.cpp"
)

add_executable("P4" ${P4_HEADERS} ${P4_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})

SET( P5_HEADERS
    # "include/kbright2/p1/..."
)

SET( P5_SRCS
    "src/kbright2/p5/p5.cpp"
)

add_executable("P5" ${P5_HEADERS} ${P5_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})


SET( P6_HEADERS
    # "include/kbright2/p1/..."
)

SET( P6_SRCS
    "src/kbright2/p6/p6.cpp"
)

add_executable("P6" ${P6_HEADERS} ${P6_SRCS}
    ${LIB_HEADERS} ${LIB_SRCS}
    ${PROV_HEADERS} ${PROV_SRCS})

add_subdirectory(test)
