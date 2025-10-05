if(BUILD_TOOLS)
    add_subdirectory(heady)

    set(HEADY_EXE "$<TARGET_FILE:heady>")
    set(GENERATED_HEADER "${CMAKE_SOURCE_DIR}/demo/CliLib.hpp")

    add_custom_command(
        OUTPUT "${GENERATED_HEADER}"
        COMMAND "${HEADY_EXE}"     
            --define "HEADY_HEADER_ONLY"
            --source "${CMAKE_SOURCE_DIR}/lib"
            --output "${GENERATED_HEADER}"
            --recursive
            --excluded "CMakeLists.txt"
        DEPENDS heady
        COMMENT "Generating single header file with heady"
    )
endif()