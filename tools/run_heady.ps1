# run_heady.ps1
<#
.SYNOPSIS
    Runs the Heady executable with default options.
.DESCRIPTION
    This script executes the Heady tool located in build/tools/heady
    using a predefined set of options.
#>

# Path to the executable (relative to this script)
$headyExe = Join-Path -Path $PSScriptRoot -ChildPath "..\build\tools\heady\Heady.exe"

# Default arguments
$argsList = @(
    "--define", "HEADY_HEADER_ONLY",
    "--source", ".\lib",
    "--output", "build\CliLib.hpp"
    "--recursive",
    "--excluded CMakeLists.txt"
)

# Execute
& $headyExe @argsList