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

