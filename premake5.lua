include "dependencies.lua"

workspace "Pine"
    location "build"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    --startproject "Sandbox"
--outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
outputdir = "%{cfg.buildcfg}"

group "Dependencies"
    include "vendor/Glad"
    include "vendor/imgui"
group ""

group "Core"
    include "Pine"
group ""

group "Misc"
    include "Sandbox"
group ""
