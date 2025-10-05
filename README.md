[![CMake Ubuntu Clang](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-clang.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-clang.yml)
[![CMake Ubuntu GCC](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-gcc.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-gcc.yml)
[![CMake Windows MSVC](https://github.com/Dr0f0x/CliLib/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/windows-msvc.yml)
[![heady build](https://github.com/Dr0f0x/CliLib/actions/workflows/heady.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/heady.yml)
[![codecov](https://codecov.io/gh/Dr0f0x/CliLib/graph/badge.svg?token=Q62QP5D61I)](https://codecov.io/gh/Dr0f0x/CliLib)

### Folder Structure: ###

```
CliLib/
├── lib/
│   ├── parsing/            # parsing for entered command
│   │   ├── CMakeLists.txt
│   │   ├── ...
│   ├── commands/           # command structure
│   │   ├── CMakeLists.txt
│   │   ├── ...
│   ├── logging/            # output to console and optionally to file
│   │   ├── CMakeLists.txt
│   │   ├── ...
│   ├── CMakeLists.txt
│   ├── clibase.h           #base for cli app
│   └── clibase.cpp
└── demo/                   # demo project
    ├── CMakeLists.txt
    └── main.cpp
```

### Idea ###

Library with a modular build system for creating CLI applications, where one can create commands, attach flags and parameters to them etc. and launch it with a single method.

### Class Structure ###

``CLIBase`` - MainObject; offers methods for registering new commands and for starting the actual actual cli (maybe the option to add functions that should be called on startup = init as well)

``Command`` - basic structure for commands, has an identifier (the command text) and arguments that can be passed with it`

Command sshould contain a descirption and automatically generated information from their parameters to be used with the -- help flag as well.

***Arguments***

**``StandardArgument``** Argument that needs to be passed with a command per se (eg. ``main.exe add 1 2`` : 1 and 2 would be the StandardArgument) a command can only have one of these.

**``OptionalArgument``** Optional Argument that could be passed with an additional flag (e.g ``main.exe run --context options.txt``). Should have a long and short form (--context and -c)

**``FlagArgument``** Optional flag that can be passed with a command (e.g ``main.exe run --fast``), basically corresponds to a bool value or the slection of an value from an enum (loglevel for example). Should have a long and short form (--context and -c)

These arguments should somehow contain help text/ a description for the CLI to automatically display when the --help or -h flag is used. Except for the StandardArgument which must always come first the order of these should be irrelevant.

***Parameter***

The values being passed with an argument should be abstracted using Parameter classes, for example in ``main.exe add 1 2`` 1 and 2 are parameters so is options.txt in ``main.exe run --context options.txt`.

Parameters specify a type they expect (e.g number or string) and are generally order dependent as without that verifying correct types would be quite impossible. Additionally a parameter should be able to be repeated (infinitely) to pass a list of sorts, however in that case he must be the last param of the argument for similar reason as above (one couldnt differentiate the end from a wrong type).

**Style For Documentation**

Optional arguments are wrapped in [    ], same with optional paramters to these arguments or to the command itself (if a parameter for an arguemnt is optional u get the nested structure [--arg [param]])

Required arguments are wrapped in <    >, same with optional paramters to these arguments or to the command itself (if a parameter for an arguemnt is optional u get the nested structure <--arg <param>>)

if an argument or parameter can be repeated it is suffixed with ... (like the params notation in many programming languages), e.g <--arg>... or [param]... or for sargs with params [--arg [param]]...

if there are multiple ways to call an arg like -h and --help they are seperated by commata, e.g [-h,--help]

args or params can have default values that are displayed in the Options section

command should have short descirption and long description (to be used with general help and command specific help)
for each arg a usage comment that is to be used in Options section, like 

Options:
    -h --help Usage comment here

TODO properly define dll api with
#pragma once

// Define EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
  #ifdef WIN_EXPORT
    // Exporting...
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllexport))
    #else
      #define EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllimport))
    #else
      #define EXPORTED __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define NOT_EXPORTED
#else
  #if __GNUC__ >= 4
    #define EXPORTED __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define EXPORTED
    #define NOT_EXPORTED
  #endif
#endif

if(MSVC)
    # Microsoft Visual C++
    target_compile_definitions(${LIBRARY_NAME_SHARED} PRIVATE WIN_EXPORT)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang and AppleClang
    add_compile_options(-fvisibility=hidden)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC
    message("setting default vis to hidden")
    set_target_properties(${LIBRARY_NAME_SHARED} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${LIBRARY_NAME_SHARED} PROPERTIES VISIBILITY_INLINES_HIDDEN YES)
endif()

install(TARGETS ${LIBRARY_NAME_SHARED}
        EXPORT CliLibTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/lib/include/ DESTINATION include)

**current Group restrictions**

- if two positional arguments are in a a mutually exclusive group and one is optional, the parser still always attempts to parse it (it isnt skipped), this may be fixable or not

cmake -DCMAKE_INSTALL_PREFIX="C:/Users/dczek/Desktop/C_C++/CliLib/install" ..
>> cmake --build . --target install