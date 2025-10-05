# Create an executable
add_executable(${DEMO_OUTPUT_NAME}
    main.cpp
)

add_custom_target(generate_header ALL DEPENDS "${GENERATED_HEADER}")
add_dependencies(${DEMO_OUTPUT_NAME} generate_header)

target_sources(${DEMO_OUTPUT_NAME}
    PRIVATE
    "${GENERATED_HEADER}")