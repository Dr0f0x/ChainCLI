<#
.SYNOPSIS
    Utility script to run various tools like Heady and gcovr.
.DESCRIPTION
    This script provides functions to:
    - Run Heady with default options
    - Run gcovr for code coverage
#>
param (
    [ValidateSet("heady", "gcovr", "license")]
    [string]$Action
)

#-----------------------------
# Function: Run Heady
#-----------------------------
function Run-Heady {
    [CmdletBinding()]
    param (
        [string]$SourceDir = ".\lib",
        [string]$OutputFile = "demo\CliLib.hpp"
    )

    $headyExe = Join-Path -Path $PSScriptRoot -ChildPath "..\build\tools\heady\heady.exe"

    $argsList = @(
        "--define", "HEADY_HEADER_ONLY",
        "--source", $SourceDir,
        "--output", $OutputFile,
        "--recursive",
        "--excluded", "CMakeLists.txt"
    )

    Write-Host "Running Heady..."
    & $headyExe @argsList
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Heady execution failed with exit code $LASTEXITCODE"
    } else {
        Write-Host "Heady completed successfully."
    }
}

#-----------------------------
# Function: Run Gcovr
#-----------------------------
function Run-Gcovr {
    [CmdletBinding()]
    param (
        [string]$RootDir = ".",
        [string]$Filter = "lib/",
        [string]$OutputHtml = "build/coverage/coverage.html"
    )
    # Ensure the output directory exists
    $outputDir = Split-Path $OutputHtml
    if (-Not (Test-Path $outputDir)) {
        New-Item -ItemType Directory -Path $outputDir | Out-Null
        Write-Host "Created directory: $outputDir"
    }

    Write-Host "Running gcovr..."
    & gcovr -r $RootDir --filter $Filter --html-details $OutputHtml --cobertura -o build/coverage/coverage.xml --exclude '_deps'
    if ($LASTEXITCODE -ne 0) {
        Write-Error "gcovr execution failed with exit code $LASTEXITCODE"
    } else {
        Write-Host "Coverage report generated at $OutputHtml"
    }
}

#-----------------------------
# Function: Run Gcovr
#-----------------------------
function Run-AddLicense {
    [CmdletBinding()]
    param (
        [string]$RunDir = "./lib",
        [string]$Ignore = "**/*.txt",
        [string]$Name = "Dominik Czekai Czekai",
        [string]$Year = "2025"
    )

    Write-Host "Running addlicense..."
    & addlicense  -c $Name -v -y $Year -ignore $Ignore $RunDir
}

#-----------------------------
# Main logic
#-----------------------------

switch ($Action) {
    "heady" { Run-Heady }
    "gcovr" { Run-Gcovr }
    "license" { Run-AddLicense }
    default { Write-Host "Please specify -Action heady, -Action license or -Action gcovr" }
}