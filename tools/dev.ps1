[CmdletBinding()]
param(
    [ValidateSet("build", "test", "benchmark", "viewer", "headless", "configure")]
    [string]$Action = "build",
    [ValidateSet("Debug", "Release", "RelWithDebInfo")]
    [string]$Configuration = "Debug",
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$RunArguments
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ViewerBuild = Join-Path $ProjectRoot "build/viewer"
$HeadlessBuild = Join-Path $ProjectRoot "build/local"

function Configure-Viewer {
    & cmake -S $ProjectRoot -B $ViewerBuild -G "Visual Studio 17 2022" -A x64 `
        -DCOCSIM_BUILD_VIEWER=ON -DCOCSIM_BUILD_PYTHON=OFF -DCOCSIM_BUILD_TESTS=ON
    if ($LASTEXITCODE -ne 0) { throw "Configuration Viewer failed." }
}

function Configure-Headless {
    & cmake -S $ProjectRoot -B $HeadlessBuild -G "Visual Studio 17 2022" -A x64 `
        -DCOCSIM_BUILD_VIEWER=OFF -DCOCSIM_BUILD_PYTHON=OFF -DCOCSIM_BUILD_TESTS=ON
    if ($LASTEXITCODE -ne 0) { throw "Configuration headless failed." }
}

function Ensure-ViewerConfigured {
    if (-not (Test-Path (Join-Path $ViewerBuild "CMakeCache.txt"))) { Configure-Viewer }
}

function Ensure-HeadlessConfigured {
    if (-not (Test-Path (Join-Path $HeadlessBuild "CMakeCache.txt"))) { Configure-Headless }
}

function Build-Viewer {
    Ensure-ViewerConfigured
    # SDL contient beaucoup d'unités MSVC. Une compilation séquentielle évite
    # les collisions de PDB observées dans les builds Debug parallèles.
    # CTest's CLI fixtures execute `cocsim` from this same build tree. Build it
    # alongside the Viewer so an identifier/catalogue edit cannot leave those
    # headless tests running a stale executable.
    & cmake --build $ViewerBuild --config $Configuration --target cocsim cocsim_viewer cocsim_tests cocsim_content_registry_tests cocsim_scenario_io_tests cocsim_determinism_tests cocsim_deployment_replay_tests cocsim_targeting_tests cocsim_permanent_building_tests cocsim_air_defense_tests cocsim_monolith_tests cocsim_super_wall_breaker_tests cocsim_super_barbarian_tests cocsim_performance_smoke_tests cocsim_viewer_thread_tests cocsim_viewer_selection_tests cocsim_viewer_projectile_visual_tests cocsim_viewer_demo_scenario_tests --parallel 1
    if ($LASTEXITCODE -ne 0) { throw "Viewer build failed." }
    & cmake --build $ViewerBuild --config $Configuration --target cocsim_super_giant_tests cocsim_super_archer_tests cocsim_rocket_balloon_tests cocsim_inferno_dragon_tests cocsim_super_wizard_tests cocsim_super_minion_tests cocsim_super_bowler_tests cocsim_super_dragon_tests --parallel 1
    if ($LASTEXITCODE -ne 0) { throw "Super Troop focused test build failed." }
}

switch ($Action) {
    "configure" { Configure-Viewer }
    "build" { Build-Viewer }
    "test" {
        Build-Viewer
        & ctest --test-dir $ViewerBuild -C $Configuration --output-on-failure
        if ($LASTEXITCODE -ne 0) { throw "Tests failed." }
    }
    "benchmark" {
        Ensure-ViewerConfigured
        & cmake --build $ViewerBuild --config $Configuration --target cocsim_performance_smoke_tests --parallel 1
        if ($LASTEXITCODE -ne 0) { throw "Performance smoke build failed." }
        & ctest --test-dir $ViewerBuild -C $Configuration -R cocsim_performance_smoke_tests --output-on-failure -V
        if ($LASTEXITCODE -ne 0) { throw "Performance smoke failed." }
    }
    "viewer" {
        Build-Viewer
        $Executable = Join-Path $ViewerBuild "$Configuration/cocsim_viewer.exe"
        & $Executable @RunArguments
        if ($LASTEXITCODE -ne 0) { throw "Viewer exited with code $LASTEXITCODE." }
    }
    "headless" {
        Ensure-HeadlessConfigured
        & cmake --build $HeadlessBuild --config $Configuration --target cocsim cocsim_tests --parallel 1
        if ($LASTEXITCODE -ne 0) { throw "Headless build failed." }
        $Executable = Join-Path $HeadlessBuild "$Configuration/cocsim.exe"
        # The demo needs longer than six seconds of logical time to resolve.
        # Let the CLI use its scenario duration so this convenience command
        # reports a completed deterministic battle rather than the expected
        # `active` (exit code 3) checkpoint result.
        if ($RunArguments.Count -eq 0) { $RunArguments = @("demo", "--trace") }
        & $Executable @RunArguments
        if ($LASTEXITCODE -ne 0) { throw "Headless simulation exited with code $LASTEXITCODE." }
    }
}
