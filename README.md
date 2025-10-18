<p align="center">
  <img src="docs/ChainCliLogoTransparent.svg" alt="Description" style="max-width: 600px; width: 80%; padding-right=30px">
</p>

[![CMake Ubuntu Clang](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-clang.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-clang.yml)
[![CMake Ubuntu GCC](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-gcc.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/ubuntu-gcc.yml)
[![CMake Windows MSVC](https://github.com/Dr0f0x/CliLib/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/windows-msvc.yml)
[![heady build](https://github.com/Dr0f0x/CliLib/actions/workflows/heady.yml/badge.svg)](https://github.com/Dr0f0x/CliLib/actions/workflows/heady.yml)
[![codecov](https://codecov.io/gh/Dr0f0x/ChainCLI/graph/badge.svg?token=Q62QP5D61I)](https://codecov.io/gh/Dr0f0x/ChainCLI)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7672ce67e13e4ca184b10fbfb4805a94)](https://app.codacy.com/gh/Dr0f0x/ChainCLI/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

> **Quick Disclaimer at the start:** This is both my first real Cpp project as well as the first project I actually made public and I probably committed numerous mistakes in both cases, so if you notice anything please create an issue about so I can attempt to fix it. Thanks!

> For its single file build this project uses an adapted version of [Heady](https://github.com/JamesBoer/Heady) by James Boer a simple utitility that is definitely worth a try if you intend to do something similar.

**ChainCLI** is a modern C++20 command-line interface library designed to make building CLI applications simple, intuitive, and maintainable. With its fluent API and method chaining approach, ChainCLI handles all the complex command-line parsing, help generation, and logging infrastructure so you can focus on implementing your application's core functionality.

## Key Features

- **Fluent API**: Build your CLI with intuitive method chaining
- **Automatic Documentation**: Generate comprehensive help text automatically
- **Type-Safe Arguments**: Strongly typed argument parsing with custom type support
- **Command Hierarchies**: Support for nested subcommands and complex CLI structures
- **Extensible**: Easy to extend with custom types, formatters, and handlers

For a more detailed overview than this README can give definitely check out the [demo folder](https://github.com/Dr0f0x/ChainCLI/tree/v1.0.0-alpha/demo) or have a look at the [API-Reference](https://dr0f0x.github.io/ChainCLI/).

## Table of Contents

- [Quick Start](#quick-start)
- [Commands](#commands)
- [Arguments](#arguments)
- [Argument Groups](#argument-groups)
- [Cli Context](#cli-context)
- [Command Docstrings](#command-docstrings)
- [CliApp Configuration](#cliapp-configuration)
- [Logging](#logging)
- [Docformatters](#docformatters)
- [Custom Types](#custom-types)

## Quick Start

The idea behind this library was to provide a very verbose and easy to understand way to create CLI-Applications, where everything that has to do with the command line interaction is done for you (create help documentation/route commands/logging), so you only have to write the actual logic of your application (e.g the interesting part). To achieve this it heavily relies on the name giving method chaining. To use it simply include the single header file ```chain_cli.hpp``` in your project.

In general, the procedure is as follows (see the demos for more specific examples):

#### 1) Create a new CliApp instance from a CliConfiguration that carries your application-specific settings

```cpp
auto config = cli::CliConfig();
config.title = "Calculator Demo";
config.executableName = "calculator_demo";
config.description = "Calculator Demo to test the CLI Library";
config.version = "1.0.0";

auto cliApp = cli::CliApp(std::move(config));
```

#### 2) Customize behaviour by adding your own/default handlers/formatters to the logger or docwriter
```cpp
auto &logger = cliApp.Logger();
logger.setLevel(LogLevel::TRACE); //the default would be info

// Attach a file handler (logs everything to one file) - by default only a console handler is added
logger.addHandler(std::make_unique<FileHandler>("app.log", std::make_shared<BasicFormatter>(), LogLevel::TRACE));
```

#### 3) Define your commands with the desired arguments and add them to the application
```cpp
auto addCommand = std::move(cli::commands::Command("add")
    .withShortDescription("Add the provided numbers")
    .withLongDescription("Adds all numbers given with the <summand> parameter and prints "
                         "the result to the console.")
    .withPositionalArgument(cli::commands::createPositionalArgument<double>("summand")
                                .withOptionsComment("All the numbers to add")
                                .withRequired(true)
                                .withRepeatable(true))
    .withExecutionFunc(add));
cliApp.withCommand(std::move(addCommand));
```

The command defined above can then be run via `<executable_name> add` and produces the help documentation:
```
calculator_demo add <summand>...

Adds all numbers given with the <summand> parameter and prints the result to the console.

Options:
<summand>...           All the numbers to add
```

#### 4) Use the generated CliContext with the parsed arguments in your own logic
```cpp
void add(const cli::CliContext &ctx)
{
    double res = 0;
    for (const auto &val : ctx.getRepeatableArg<double>("summand"))
    {
        res += val;
    }
    ctx.Logger().info("Result: {}", res);
}
```

#### 5) Call the run method of your CliApp with the passed command line arguments

Either using the predefined macro
```cpp
int main(int argc, char *argv[])
{
    RUN_CLI_APP(cliApp, argc, argv);
}
```
which expands to 
```cpp
try
{
    return cliInstance.run(argc_, argv_);
}
catch ()
{
    cliInstance.Logger().error() << e.what() << std::endl;
    std::abort();
}
```

or with your own way of calling CliApp::run.

> To see library internal logs define ```CHAIN_CLI_VERBOSE``` when compiling.

## Commands

Commands are created with a unique identifier (=the name of the command to invoke it from the command line) and each command can have its own subcommands that can be added with the ```withSubcommand``` method. That means overall the commands are structured in a tree that can be traversed via the identifiers.

Below is an example of what the structure of a CliApp could look like

```
myapp
├── math
│   ├── add <numbers>...
│   ├── subtract <a> <b>
│   └── calc
│       ├── sqrt <number>
│       └── pow <base> <exponent>
└── file
    ├── read <filename>
    ├── write <filename> <content>
    └── compress
       ├── zip <files>...
       └── gzip <file>
```

This structure would allow commands like:
- `myapp math add 1,2,3,4`
- `myapp math calc sqrt 16`
- `myapp file compress zip file1.txt,file2.txt`

Each command in the tree can have its own arguments, options, and execution logic, while sharing the common CLI infrastructure provided by the library. The root command (fittingly called myapp here, basically corresponds to the executable itself) can be configured through
```cpp
CliApp::getMainCommand(); //returns a pointer to the root command
```

## Arguments

Each command can have different arguments added to it that can be configured in itself. There are three types of arguments provided that can be added to a command with the corresponding methods

**PositionalArguments** - ```Command::withPositionalArgument```

Arguments that are parsed based on the order they were passed to the application and require at least a name (-> constructor argument)

```cpp
command.withPositionalArgument(cli::commands::createPositionalArgument<double>("add")
                                .withOptionsComment("All the numbers to add")
                                .withRequired(true)
                                .withRepeatable(true))
``` 
Displayed in the help messages like ```<positional_name>```

**OptionArguments** - ```Command::withOptionArgument```

Arguments that are parsed based on a preceding flag (usually a long one like ```--type``` and a short one like ```-t```) and require at least a (long) name and a value name (-> the constructor arguments)

```cpp
command.withOptionArgument(cli::commands::OptionArgument<double>("--bound", "lowest")
                            .withShortName("-b")
                            .withOptionsComment("specify a lower bound to display a message if the subtraction process crosses it"))
``` 
Displayed in the help messages like ```[--option_name,-short_option_name <value_name>]```

**FlagArguments** - ```Command::withFlagArgument```

Arguments that only check if the specified flag is present or not (can be used like a toggle) and require at least a (long) name and a short name (-> constructor arguments)

```cpp
.withFlagArgument(cli::commands::FlagArgument("--verbose", "-v")
                    .withOptionsComment("Enable verbose output for the division process")
                    .withRequired(false))
``` 


The first two of these also immediately parse corresponding input parts to a value of the type that was provided when creating the argument. All of the arguments can be required, repeatable and can have a options comment that is displayed in the OPTIONS section of the help command for the corresponding command. Repeatable arguments can be provided in a ','-separated list and are then parsed as a vector instead of a single instance.

> Instead of using the chaining methods to create the arguments you can also provide the values to the constructor directly like it is done in the following example:

```cpp
.withPositionalArgument(cli::commands::PositionalArgument<double>(
    "dividend", "The number that is divided", true, false))
.withPositionalArgument(cli::commands::PositionalArgument<double>(
    "divisor", "The numbers to divide by", true, true))
.withFlagArgument(cli::commands::FlagArgument(
    "--remainder", "-r", "Divide using remainders instead of precise", false))
```

In my opinion this is a lot less verbose but it depends on your preferences. 

## Argument Groups

Arguments can be put into groups, specifically into mutually exclusive or mutually inclusive ones. This can be done by using the corresponding chaining methods ```Command::withExclusiveGroup``` or ```Command::withInclusiveGroup``` and passing the arguments you want to have in the group.
Inclusive groups require all the arguments in the group to be present if one of them is, while exclusive groups allow only one of the arguments to be present.

```cpp
.withExclusiveGroup(cli::commands::OptionArgument<int>("--value", "number")
                                .withShortName("-v")
                                .withOptionsComment("First argument in exclusive group")
                                .withRequired(false),
                            cli::commands::OptionArgument<std::string>("--name", "text")
                                .withShortName("-n")
                                .withOptionsComment("Second argument in exclusive group")
                                .withRequired(false))
```

```cpp
.withInclusiveGroup(cli::commands::OptionArgument<int>("--value", "number")
                                .withShortName("-v")
                                .withOptionsComment("First argument in inclusive group")
                                .withRequired(false),
                            cli::commands::OptionArgument<std::string>("--name", "text")
                                .withShortName("-n")
                                .withOptionsComment("Second argument in inclusive group")
                                .withRequired(false))
```

> Using this, one can quite easily create uncallable commands, for example creating a mutually exclusive group where two of the arguments in it are required. The library doesn't check for this and so will not provide any warnings or something similar.

## Cli Context

To pass the parsed Arguments to the implemented logic the library uses ```CliContext``` instances. These provide access to them using the corresponding methods like ```CliContext::getPositionArg``` or ```CliContext::getArg``` (internally this one searches through flag, positional and option args). Note that you have to provide the argument name that was specified when creating the argument to retrieve its value. As arguments that are not required don't have to be present, there are methods to check if they were provided, namely ```CliContext::isOptionArgPresent``` and equivalents for the other argument types.

```cpp
// Example: Accessing different argument types in your command function
void subtract(const cli::CliContext &ctx)
{
    auto minuend = ctx.getPositionalArg<double>("minuend");
    double bound = minuend + 1; // just to have an initial value
    bool boundPresent = ctx.isOptionArgPresent("--bound");

    for (const auto &val : ctx.getRepeatableArg<double>("subtrahend"))
    {
        minuend -= val;
        if (boundPresent && minuend < bound)
        {
            ctx.Logger().info("Subtraction crossed the bound of {}: current value is {}", bound,
                            minuend);
        }
    }
    ctx.Logger().info("Result: {}", minuend);
}
```

Additionally the ```CliContext``` objects carry a reference to the CliApps Logger instance which can be accessed through ```CliContext::Logger``` so you can use the configured Logger in your own logic.

## Command Docstrings

Internally the text that is printed for help messages is called a docstring and commands have both a short and a long docstring. The first one is used in help message for the whole app (printed when the executable is invoked without a valid command to call or with <executable> --help) and the second one in the help message for each single command (printed via <executable> <command_name> --help).

**example of an app help message**
```
Calculator Demo to test the CLI Library

calculator_demo add <summand>... 
Add the provided numbers

calculator_demo div <dividend> <divisor>... [--remainder,-r] 
Divide the first argument by all args after it

calculator_demo mult <factor>... 
Multiply the provided numbers

calculator_demo sub <minuend> <subtrahend>... [--bound,-b <lowest>] 
Subtracts all the numbers after the first one from it

Use <command> --help|-h to get more information about a specific command
```

**example of a command help message**
```
calculator_demo sub <minuend> <subtrahend>... [--bound,-b <lowest>] 

Subtracts all numbers given with the <subtrahend> parameter from the number given with the <minuend> parameter and prints the result to the console.

Options:
<minuend>              The number that is subtracted from
<subtrahend>...        All the numbers to subtract
--bound,-b <lowest>    specify a lower bound to display a message if the subtraction process crosses it
```

The text in the middle is the short or long description of the command (can be specified for each command with the chaining methods).
The display in the first line and the textual representation of the arguments is modeled after [docopt](http://docopt.org/) meaning:

- required positional arguments are enclosed in ```<>``` and optional positional arguments in ```[]```. They display their name in the brackets: ```<minuend>```

- required option arguments are enclosed in ```()``` and optional positional arguments in ```[]```. They display their name followed by a semicolon and the short name (if one was specified) as well as the value_name enclosed in ```<>```: ```[--bound,-b <lowest>]```

- flag arguments are always optional and enclosed in ```[]```.  They display their name and short name (if specified) in the same fashion that option arguments do: ```[--remainder,-r]```

- repeatable arguments are differentiated by adding ```...``` after the argument itself : ```<summand>...```

- like option arguments, argument groups are enclosed by ```()``` if required and ```[]``` otherwise. Additionally exclusive groups separate their arguments by ```|``` whereas inclusive groups simply use spaces. Inclusive groups are required as soon as one of their arguments is required and exclusive groups if every one of their arguments is required : ```[[--value,-v <number>] | [--name,-n <text>]]```

> The order of the arguments in the display is determined by the order the arguments were added to the command!

## CliApp Configuration

The ```CliConfig``` struct is used to configure the CliApplication and change default presets. You can either pass your own instance when creating the CliApp or later edit the configuration via ```CliApp::getConfig```. Examples of settings that can be changed this way are the optionsWidth the help messages use for the line length in the Options section and the alignment there or the repeatableDelimiter used to split repeatable arguments (default ","), as well as the executable name or similar project specific details.

## Logging

The library uses a simple logging module that works by creating a single logger instance and attaching handlers with their own formatters to it. Each Handler is responsible for outputting a message that was formatted by its formatter (the default formatters provided are the message only formatter and one that includes timestamp and loglevel) to a different target (the default handlers provided target either the console or a file).

Both the logger itself and all the handlers have a minimum level and ignore all logs that are below it. The one of the logger can be set with ```Logger::setLevel``` for the handlers they have to be specified before adding them to the logger, e.g in the constructor (the default console handler has Trace as its level).

Moreover a simple LogStyle struct (basically a map of LogLevel to strings) is used to style each loglevel with ANSI-escape-sequences. By default this is only used to color the output for the different levels, like so

<span style="color: gray">This is a TRACE message</span>  
<span style="color: gray">This is a VERBOSE message</span>  
<span style="color: blue">This is a DEBUG message</span>  
This is an INFO message  
<span style="color: orange">This is a WARNING message</span>  
<span style="color: green">This is a SUCCESS message</span>  
<span style="color: red">This is an ERROR message</span>

You can easily write your own handler or formatter by extending the corresponding abstract base class (```AbstractHandler``` or ```AbstractFormatter```). If needed one can also write their own implementation of the ```AbstractLogger``` and pass it when creating the CliApp to use instead of the one the library provides.

> The streams available with ```Logger::info```, ```Logger::debug``` and so on have to be manually flushed using ```std::flush```!

## Docformatters

To generate its help messages the library uses one central class the ```Docwriter``` which has references to different types of docformatters: One for each argument type (positional/option/flag), one for a single command and one for the application as a whole.

At the start of the program both the short and long docstring for each command is built by first retrieving the docstrings (called ArgumentDocString for the display with the brackets in the first line and OptionsDocString for the line in the OPTIONS section) for the arguments which results in calls to the docwriter and their regarding formatters. These are then used by a the commandFormatter to build and set the long/short docstring per command.

If a help message then needs to be printed (either the one for the app or for a single command) the already built docstrings of the commands are used by the AppFormatter to produce the final output.

All of these steps can be fully customized by replacing the default formatters with your own implementation of the abstract base classes (```AbstractCliAppDocFormatter```, ```AbstractCommandFormatter``` and ```AbstractArgDocFormatter```) like below:

```cpp
class CustomOptionFormatter : public cli::commands::docwriting::DefaultOptionFormatter
{
public:
    std::string generateArgDocString(const cli::commands::OptionArgumentBase &argument,
                                     const cli::CliConfig &configuration) override
    {
        std::ostringstream builder;
        builder << "[Option: " << argument.getName();
        if (argument.isRequired())
            builder << ", Required";
        if (argument.isRepeatable())
            builder << ", Repeatable";
        builder << "]";
        return builder.str();
    }

    std::string generateOptionsDocString(const cli::commands::OptionArgumentBase &argument,
                                         const cli::CliConfig &configuration) override
    {
        std::ostringstream builder;
        builder << DefaultOptionFormatter::generateOptionsDocString(argument, configuration);

        if (argument.isRequired())
            builder << " (Required)";
        if (argument.isRepeatable())
            builder << " (Repeatable)";
        return builder.str();
    }
};

int main(int argc, char *argv[])
{
    auto cliApp = cli::CliApp("DocFormatterDemo");
    
    initCommands(cliApp);

    cliApp.getDocWriter().setAppFormatter(std::make_unique<CustomAppDocFormatter>());
    cliApp.getDocWriter().setOptionFormatter(std::make_unique<CustomOptionFormatter>());
}
```

## Custom Types

To parse the input string to actual values the library simply uses the ```>>``` operator, therefore you simply have to provide an appropriate overload of that operator for the parsing module to use.

```cpp
struct CustomType
{
    friend std::istream &operator>>(std::istream &is, CustomType &ct)
    {
        //Custom parse implementation
    }
};
```


Thats it! If some things are not fully clear yet, try having a look at the [demo projects](https://github.com/Dr0f0x/ChainCLI/tree/v1.0.0-alpha/demo) or check out the detailed [API-Reference](https://dr0f0x.github.io/ChainCLI/).