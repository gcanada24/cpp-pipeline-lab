# Build and run one lesson from any PowerShell window (no Developer PowerShell needed).
#   .\tools\run.ps1 01            -> Debug build of lessons/01-*, then runs it
#   .\tools\run.ps1 01 -Release   -> optimized build (use for the performance lessons)
param(
    [Parameter(Mandatory = $true)][string]$Lesson,
    [switch]$Release
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$preset = if ($Release) { 'release' } else { 'debug' }

# Load the MSVC environment (cl, cmake, ninja) if this shell doesn't have it yet.
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vs = & $vswhere -latest -products * -property installationPath
    Import-Module "$vs\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
    Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64' | Out-Null
}

Push-Location $root
try {
    cmake --preset $preset --log-level=WARNING | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "`nCMake configure failed (see above)." -ForegroundColor Red; exit 1 }
    cmake --build --preset $preset --target "lesson$Lesson"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "`nBuild failed. Read the first 'error' line above: C#### = compile error, LNK#### = link error." -ForegroundColor Red
        exit 1
    }
    Write-Host ''
    & "$root\build\$preset\lesson$Lesson.exe"
    exit $LASTEXITCODE
} finally {
    Pop-Location
}
