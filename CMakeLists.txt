project(3DL)

string( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# don't allow in-source builds, and protect source code!
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# show all warnings, optimize, optimize for current architecture
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3 -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -march=native")

# Standard settings are the same as release
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_RELEASE} -Wall -O3 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -march=native")

# show all warnings, don't optimize, debug symbols
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g")

# set c++-11 standard and enforce
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

OPTION (USE_OpenMP "Use OpenMP" OFF)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(USE_OpenMP ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(USE_OpenMP OFF)
endif()

IF(USE_OpenMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ENDIF()
ENDIF()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# all header files are in this folder, so include here rather than in subordinate cmake files
include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include"   # useful for calling from a parent project
    "${PROJECT_SOURCE_DIR}/thirdparty/stlplus"
)

# store list of header files in variable for further use,
# todo: make manual in order to have make detect changes..
file(GLOB HDRS
    "include/*.h"
    "thirdparty/stlplus/*.hpp"
)

# install all files in the nomoko header folder
install(FILES ${HDRS}
    DESTINATION "include/3DL"
)

# ability to switch individual things on/off
option(BUILD_EXAMPLES "Compile example/tutorial binaries" ON)
option(BUILD_LIB "Compile and install static libraries" ON)
option(BUILD_SCRIPTS "Compile utility scripts" ON)
option(BUILD_TESTS "Compile and run tests" ON)

# enable tests at root level because we have to?!
if( BUILD_TESTS )
    include(CTest)
    enable_testing()
endif( BUILD_TESTS )

# everything else happens in the subfolders
add_subdirectory(source)
add_subdirectory(thirdparty)
add_subdirectory(exampleApps)
