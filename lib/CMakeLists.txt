set(LIBRARY_OUTPUT_NAME cli)

# Create main library combining all modules
add_library(${LIBRARY_OUTPUT_NAME}
    clibase.cpp
    utils.cpp
)

# Add subdirectories for modules
add_subdirectory(parsing)
add_subdirectory(commands)
add_subdirectory(logging)

# Include path for consumers
target_include_directories(cli
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)