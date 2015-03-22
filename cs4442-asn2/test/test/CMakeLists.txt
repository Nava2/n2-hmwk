
IF (NOT KBRIGHT2_ASN2_MAIN )
    message(fatal_error "Can not configure from this directory")
ENDIF()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

SET( asn2_testsrcs
    ngram-test.cpp
    
    catch/catch.hpp
)

include_directories("include/")
include_directories("../include")

add_executable(asn2-test
    main.cpp
    ${asn2_testsrcs}
)
