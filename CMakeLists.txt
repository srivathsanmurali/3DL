project(3DL)

string( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# don't allow in-source builds, and protect source code!
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# show all warnings, optimize, optimize for current architecture
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

# show all warnings, don't optimize, debug symbols
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g")

# set c++-11 standard and enforce
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

OPTION (USE_OpenMP "Use OpenMP" ON)
FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(USE_OpenMP ON)
else()
  set(USE_OpenMP OFF)
ENDIF()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# all header files are in this folder, so include here rather than in subordinate cmake files
include_directories(
  .
  "${PROJECT_SOURCE_DIR}/thirdparty/stlplus"
  "${PROJECT_SOURCE_DIR}/thirdparty/"
)

# store list of header files in variable for further use,
# todo: make manual in order to have make detect changes..
file(GLOB HDRS
    "./*.h"
    "thirdparty/stlplus/*.hpp"
    "thirdparty/log.h"
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

#plyIO
add_library(libPlyIO plyIO.cc ${HDRS})
install(TARGETS libPlyIO DESTINATION "lib/3DL")
 
#voxelGridFilter
add_library(libVoxelGridFilter voxelGridFilter.cc ${HDRS})
install(TARGETS libVoxelGridFilter DESTINATION "lib/3DL")

# ransacPlaneDetection
add_library(libRansacPlaneDetection ransacPlaneDetection.cc ransacPlaneDetection.h)
install(TARGETS libRansacPlaneDetection DESTINATION "lib/3DL")

# everything else happens in the subfolders
add_subdirectory(thirdparty)
add_subdirectory(exampleApps)
