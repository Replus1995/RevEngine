include "custom_init.lua"
include "dependencies.lua"
workspace "RevEngine"
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

IncludeDir["Rev"] = "%{wks.location}/../Rev/Source/Public"

group "Dependencies"
    include "vendor/Glad"
    include "vendor/imgui"
group ""

group "Core"
    include "Rev"
group ""

group "Misc"
    include "Sandbox"
group ""
