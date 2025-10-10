if(BUILD_DEMO)

    # Create individual demo executables
    add_executable(calculator_demo
        calculator_demo.cpp
    )

    add_executable(argument_group_demo
        argument_group_demo.cpp
    )

    add_executable(root_command_demo
        root_command_demo.cpp
    )

    add_executable(subcommand_demo
        subcommand_demo.cpp
    )   

    add_executable(custom_type_demo
        custom_type_demo.cpp
    )

    add_executable(custom_docformatter_demo
        custom_docformatter_demo.cpp
    )

    add_executable(logger_demo
        logger_demo.cpp
    )

    target_sources(calculator_demo
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(argument_group_demo  
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(root_command_demo
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(subcommand_demo
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(custom_type_demo
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(custom_docformatter_demo
        PRIVATE
        ${GENERATED_HEADER})

    target_sources(logger_demo
        PRIVATE
        ${GENERATED_HEADER})

    # Apply clang-tidy to demo targets
    if(DEFINED CLANG_TIDY_COMMAND)
        set_target_properties(calculator_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(argument_group_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(root_command_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(subcommand_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(custom_type_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(custom_docformatter_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        set_target_properties(logger_demo PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    endif()

endif()