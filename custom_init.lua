require('vstudio')

local p = premake
local m = p.vstudio.vc2010

premake.api.register {
    name = "dounitybuild",
    scope = "config",
    kind = "boolean",
    default = false
}

local function doUnityBuild(cfg)
    if cfg.dounitybuild then
        premake.w('<EnableUnitySupport>true</EnableUnitySupport>')
    else
        premake.w('<EnableUnitySupport>false</EnableUnitySupport>')
    end
end

premake.override(m.elements, "configurationProperties", function(base, cfg)
    local calls = base(cfg)
    table.insert(calls, doUnityBuild)
    return calls
end)