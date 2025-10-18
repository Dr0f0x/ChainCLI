if(BUILD_TOOLS)
    add_subdirectory(heady)

    set(HEADY_EXE "$<TARGET_FILE:heady>")
    set(GENERATED_HEADER "${CMAKE_SOURCE_DIR}/demo/chain_cli.hpp" PARENT_SCOPE)

    add_custom_command(
        OUTPUT "${GENERATED_HEADER}"
        COMMAND "${HEADY_EXE}"
            "${CMAKE_SOURCE_DIR}/src/chaincli"     
            --license-header "${CMAKE_SOURCE_DIR}/tools/copyright_headers.txt"
            --output "${GENERATED_HEADER}"
            --recursive
            --excluded "CMakeLists.txt"
        DEPENDS heady
        COMMENT "Generating single header file with heady"
        VERBATIM
    )

    add_custom_target(generate_header
        COMMAND "${HEADY_EXE}"
            "${CMAKE_SOURCE_DIR}/src/chaincli"
            --license-header "${CMAKE_SOURCE_DIR}/tools/copyright_headers.txt"    
            --output "${GENERATED_HEADER}"
            --recursive
            --excluded "CMakeLists.txt"
        DEPENDS heady
        COMMENT "Generating single header file with heady"
        VERBATIM
    )

    # Target to clean the generated header
    add_custom_target(clean_header
        COMMAND ${CMAKE_COMMAND} -E remove -f "${GENERATED_HEADER}"
        COMMENT "Remove generated header file"
    )
endif()