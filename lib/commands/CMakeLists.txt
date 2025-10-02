target_sources(${LIBRARY_OUTPUT_NAME}
    PUBLIC
        command.h
        command.cpp
        argument.h
        positional_argument.h
        option_argument.h
        flag_argument.h
        flag_argument.cpp
        command_tree.h
        command_tree.cpp
)

add_subdirectory(docwriting)