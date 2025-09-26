cmake_minimum_required(VERSION 3.25.0)
project(
    CliLib
    VERSION 1.0
    DESCRIPTION "CLI Library"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_NAME cli)

enable_testing()

# Build the library
add_subdirectory(lib)

# Build the demo app
add_subdirectory(demo)

# Include tests
add_subdirectory(test)

# Make BUILD_SHARED_LIBS visible in GUIs like ccmake / cmake-gui
option(BUILD_SHARED_LIBS "Build libraries as shared instead of static" OFF)