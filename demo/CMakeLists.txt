# Create an executable
add_executable(${DEMO_OUTPUT_NAME}
    main.cpp
)

# Link against the library
target_link_libraries(${DEMO_OUTPUT_NAME}
    PRIVATE ${LIBRARY_NAME_STATIC}
)