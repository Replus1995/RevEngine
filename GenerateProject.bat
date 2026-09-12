@echo off
setlocal

echo ====== Generate Visual Studio Solution ======

for %%I in ("%~dp0.") do set "ROOT_DIR=%%~fI"
set "BUILD_DIR=%ROOT_DIR%\build"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo Error: Visual Studio Installer's vswhere.exe was not found.
    exit /b 1
)

for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL=%%I"
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion`) do set "VS_VERSION=%%I"

if not defined VS_INSTALL (
    echo Error: no Visual Studio installation with the C++ toolchain was found.
    exit /b 1
)

for /f "tokens=1 delims=." %%I in ("%VS_VERSION%") do set "VS_MAJOR=%%I"

if "%VS_MAJOR%"=="18" (
    set "CMAKE_GENERATOR=Visual Studio 18 2026"
    set "SOLUTION_EXTENSION=slnx"
) else if "%VS_MAJOR%"=="17" (
    set "CMAKE_GENERATOR=Visual Studio 17 2022"
    set "SOLUTION_EXTENSION=sln"
) else (
    echo Error: Visual Studio version %VS_VERSION% is not supported.
    exit /b 1
)

set "CMAKE_EXE=%VS_INSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist "%CMAKE_EXE%" (
    set "CMAKE_EXE_FOUND="
    for /f "delims=" %%I in ('where cmake 2^>nul') do if not defined CMAKE_EXE_FOUND set "CMAKE_EXE_FOUND=%%I"
    set "CMAKE_EXE=%CMAKE_EXE_FOUND%"
)

if not exist "%CMAKE_EXE%" (
    echo Error: a compatible CMake executable was not found.
    exit /b 1
)

if not defined VULKAN_SDK (
    for /f "delims=" %%I in ('where glslc.exe 2^>nul') do if not defined VULKAN_SDK for %%J in ("%%~dpI..") do set "VULKAN_SDK=%%~fJ"
)

if not exist "%VULKAN_SDK%\Include\vulkan\vulkan.h" (
    echo Error: Vulkan SDK was not found. Install it or set VULKAN_SDK.
    exit /b 1
)

echo Visual Studio: %VS_INSTALL%
echo CMake: %CMAKE_EXE%
echo Generator: %CMAKE_GENERATOR%
echo Vulkan SDK: %VULKAN_SDK%

"%CMAKE_EXE%" -S "%ROOT_DIR%" -B "%BUILD_DIR%" --fresh -G "%CMAKE_GENERATOR%" -A x64 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
if errorlevel 1 exit /b %errorlevel%

set "SOLUTION_FILE=%BUILD_DIR%\RevEngine.%SOLUTION_EXTENSION%"
echo Solution: %SOLUTION_FILE%
if not exist "%SOLUTION_FILE%" (
    echo Error: generated solution was not found: %SOLUTION_FILE%
    exit /b 1
)

if not "%~1"=="" goto DONE

set "DEVENV_EXE=%VS_INSTALL%\Common7\IDE\devenv.exe"
if not exist "%DEVENV_EXE%" (
    echo Error: Visual Studio IDE was not found: %DEVENV_EXE%
    exit /b 1
)

start "" "%DEVENV_EXE%" "%SOLUTION_FILE%"

:DONE
endlocal
