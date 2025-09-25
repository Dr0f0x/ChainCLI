# Build library (STATIC or SHARED depending on BUILD_SHARED_LIBS)
add_library(cli
    cli.cpp
)

# Public include path
target_include_directories(cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optionally, export symbols if building shared on Windows
if (BUILD_SHARED_LIBS AND WIN32)
    target_compile_definitions(cli PRIVATE CLI_EXPORTS)
endif()