target_sources(${LIBRARY_OUTPUT_NAME}
    PUBLIC
        command.h
        command.cpp
        argument.h
        argument.cpp
        command_tree.h
        command_tree.cpp
)

add_subdirectory(docwriting)