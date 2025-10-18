/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Heady.h for license details.
Copyright (c) 2018 James Boer

Modifications and integration with ChainCLI:
Copyright (c) 2025 Dominik Czekai
*/

#include <cstring>
#include <iostream>
#include <thread>

#include "Heady.h"
#define CHAIN_CLI_VERBOSE
#include "chain_cli.hpp"

using namespace cli;

void mainHandler(const CliContext &ctx)
{
    Heady::Params params(ctx.Logger());
    params.sourceFolder = ctx.getPositionalArg<std::string>("folder");
    params.output = ctx.getOptionArg<std::string>("--output");
    params.recursiveScan = ctx.isFlagPresent("--recursive");
    params.includeFileHints = ctx.isFlagPresent("--include-file-hint");
    params.useStandardIncludeGuard = ctx.isFlagPresent("--use-standard-include-guard");

    if (ctx.isOptionArgPresent("--excluded"))
        params.excluded = ctx.getOptionArg<std::string>("--excluded");

    if (ctx.isOptionArgPresent("--inline"))
        params.inlined = ctx.getOptionArg<std::string>("--inline");

    if (ctx.isOptionArgPresent("--define"))
        params.define = ctx.getOptionArg<std::string>("--define");
    
    if (ctx.isOptionArgPresent("--license-header"))
        params.licenseHeader = ctx.getOptionArg<std::string>("--license-header");

    try
    {
        Heady::GenerateHeader(params);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing source files.  " << e.what() << std::endl;
    }
}

int main(int argc, char **argv)
{
    CliConfig config = CliConfig();
    config.title = "Heady";
    config.executableName = "heady";
    config.description =
        "Heady is a tool to generate single-file amalgamated C++ header files from "
        "a folder of C++ source files.";
    config.version = "1.0.0";

    CliApp app = CliApp(std::move(config));

    auto &mainCommand = *(app.getMainCommand());
    mainCommand
        .withLongDescription(
            "Scans a folder of C++ source files (optionally recursive) and generates a single-file "
            "amalgamated "
            "header file. It processes #include directives, inlines code marked with a specific "
            "macro, and allows for excluding certain files. This is useful for creating "
            "self-contained header-only libraries.")
        .withPositionalArgument(
            commands::PositionalArgument<std::string>("folder")
                .withOptionsComment("The folder containing the C++ source files to process")
                .withRequired(true))
        .withOptionArgument(
            commands::OptionArgument<std::string>("--excluded", "files")
                .withShortName("-e")
                .withOptionsComment("A space-separated list of files to exclude from processing"))
        .withOptionArgument(
            commands::OptionArgument<std::string>("--inline", "name")
                .withShortName("-i")
                .withOptionsComment(
                    "The macro name that marks code to be inlined into the amalgamated header"))
        .withOptionArgument(
            commands::OptionArgument<std::string>("--define", "define")
                .withShortName("-d")
                .withOptionsComment("A define to add to the top of the generated header file"))
        .withOptionArgument(
            commands::OptionArgument<std::string>("--output", "file")
                .withShortName("-o")
                .withOptionsComment("The output path for the generated amalgamated header file")
                .withRequired(true))
        .withOptionArgument(
            commands::OptionArgument<std::string>("--license-header", "-lh", "regex")
                .withOptionsComment("A regex pattern to match and remove copyright/license headers from processed files"))
        .withFlagArgument(
            commands::FlagArgument("--recursive", "-r")
                .withOptionsComment("Recursively scan the source folder for C++ files"))
        .withFlagArgument(commands::FlagArgument("--include-file-hint", "-ifh")
                              .withOptionsComment("Include a file hints in the generated header"))
        .withFlagArgument(commands::FlagArgument("--use-standard-include-guard", "-usig")
                              .withOptionsComment("Use standard include guard in the generated header and not pragma once"))
        .withExecutionFunc(mainHandler);

    RUN_CLI_APP(app, argc, argv)
}