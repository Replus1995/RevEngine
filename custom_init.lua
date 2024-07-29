require('vstudio')

-- Enable Visual Studio Unity Build Support
-- Don't know why the official option 'enableunitybuild' does not take effect
local p = premake
local m = p.vstudio.vc2010

p.api.register {
    name = "unitybuild",
    scope = "config",
    kind = "boolean",
    default = false
}

local function vsUnityBuild(cfg)
    if _ACTION >= "vs2017" and cfg.unitybuild then
        m.element("EnableUnitySupport", nil, iif(cfg.unitybuild, "true", "false"))
    end
end

p.override(m.elements, "configurationProperties", function(base, cfg)
    local calls = base(cfg)
    table.insert(calls, vsUnityBuild)
    return calls
end)