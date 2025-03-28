# Run as administrator
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Warning "Please run this script as Administrator"
    Break
}

Write-Host "Setting up MSVC environment..." -ForegroundColor Green

# Kill any existing CMake processes
Get-Process -Name "cmake" -ErrorAction SilentlyContinue | Stop-Process -Force
Get-Process -Name "cl" -ErrorAction SilentlyContinue | Stop-Process -Force

# Set up Visual Studio environment
$vsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community"
$vcvarsPath = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"

if (Test-Path $vcvarsPath) {
    # Create a temporary batch file to run vcvars64.bat
    $tempBatch = Join-Path $env:TEMP "setup_vcvars.bat"
    "@echo off`ncall `"$vcvarsPath`"`nset" | Out-File -FilePath $tempBatch -Encoding ASCII
    
    # Run the batch file and capture its environment variables
    $cmd = "cmd /c `"$tempBatch`""
    $envVars = cmd /c $cmd | Where-Object { $_ -match '^([^=]+)=(.*)$' } | ForEach-Object {
        $name = $matches[1]
        $value = $matches[2]
        Set-Item -Path "Env:$name" -Value $value
    }
    
    # Clean up
    Remove-Item $tempBatch -Force
} else {
    Write-Error "Visual Studio 2022 installation not found at: $vsPath"
    exit 1
}

# Add CMake to PATH if not already there
$cmakePath = "C:\Program Files\CMake\bin"
if (Test-Path $cmakePath) {
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
    if ($currentPath -notlike "*$cmakePath*") {
        [Environment]::SetEnvironmentVariable("Path", $currentPath + ";$cmakePath", "Machine")
    }
} else {
    Write-Error "CMake installation not found at: $cmakePath"
    exit 1
}

# Verify installations
Write-Host "`nTesting CMake version:" -ForegroundColor Yellow
cmake --version

Write-Host "`nTesting MSVC compiler:" -ForegroundColor Yellow
cl

Write-Host "`nTesting C++20 support:" -ForegroundColor Yellow
@"
#include <iostream>
#include <concepts>

int main() { return 0; }
"@ | Out-File -FilePath "test.cpp" -Encoding ASCII

cl /std:c++20 test.cpp
Remove-Item "test.cpp", "test.obj", "test.exe" -Force -ErrorAction SilentlyContinue

Write-Host "`nEnvironment setup complete." -ForegroundColor Green
Write-Host "Please restart your IDE for changes to take effect." -ForegroundColor Yellow

# Keep the window open
Write-Host "`nPress any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") 