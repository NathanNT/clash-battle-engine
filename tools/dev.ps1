[CmdletBinding()]
param(
    [ValidateSet("build", "test", "viewer", "headless", "configure")]
    [string]$Action = "build",
    [ValidateSet("Debug", "Release", "RelWithDebInfo")]
    [string]$Configuration = "Debug",
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$RunArguments
)
$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build/empty"
& cmake -S $ProjectRoot -B $BuildDir -G "Visual Studio 17 2022" -A x64 -DCOCSIM_BUILD_VIEWER=ON -DCOCSIM_BUILD_TESTS=ON -DCOCSIM_BUILD_PYTHON=OFF
if ($LASTEXITCODE -ne 0) { throw "Configuration failed." }
if ($Action -eq "configure") { exit 0 }
& cmake --build $BuildDir --config $Configuration --parallel 1
if ($LASTEXITCODE -ne 0) { throw "Build failed." }
if ($Action -eq "test") {
    & ctest --test-dir $BuildDir -C $Configuration --output-on-failure
    if ($LASTEXITCODE -ne 0) { throw "Tests failed." }
} elseif ($Action -eq "viewer") {
    $Executable = Join-Path $BuildDir "$Configuration/cocsim_viewer.exe"
    & $Executable @RunArguments
    if ($LASTEXITCODE -ne 0) { throw "Viewer failed." }
} elseif ($Action -eq "headless") {
    $Executable = Join-Path $BuildDir "$Configuration/cocsim.exe"
    if ($RunArguments.Count -eq 0) { $RunArguments = @("demo") }
    & $Executable @RunArguments
    if ($LASTEXITCODE -ne 0) { throw "Headless failed." }
}
