cmake_minimum_required(VERSION 3.1.3)

include(cmake/base.cmake)
include(cmake/msvc-specific.cmake)

set(PROJECT_NAME cdd)
set(PROJECT_DESCRIPTION "The library cdd is a C implementation of the Double Description Method of Motzkin et al. for generating all vertices (i.e. extreme points) and extreme rays of a general convex polyhedron in Rd given by a system of linear inequalities.")
set(PROJECT_URL "https://github.com/cddlib/cddlib")
set(PROJECT_DEBUG_POSTFIX "_d")
set(PROJECT_VERSION "0.94.10") # 094j
set(INSTALL_GENERATED_HEADERS OFF CACHE BOOL "Generate and install standard headers")

set(CXX_DISABLE_WERROR True)
option(USE_GMP "Set to true to use gmp rational" OFF)

setup_project()

if(MSVC)
    set(CMAKE_MSVCIDE_RUN_PATH "\$(SolutionDir)/src/\$(Configuration)")
endif(MSVC)

add_subdirectory(lib-src)

setup_project_finalize()
setup_project_package_finalize()
