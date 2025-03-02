ROOT_PATH = function(path)
    return string.format("%s/%s", _MAIN_SCRIPT_DIR, path)
end

PROJ_DIR = ROOT_PATH "projects"
DEP_DIR  = ROOT_PATH "dependencies"
BIN_DIR  = ROOT_PATH "_build/bin/%{cfg.buildcfg}-%{cfg.platform}"
OBJ_DIR  = ROOT_PATH "_build/obj/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}"

WORKSPACE_NAME = "Cereal"
START_PROJECT  = "Test"

print("Executing workspace.lua")
include 'workspace.lua'

function platform_defines()
    print("Setting platform defines")

    filter {"configurations:debug"}
        defines{"DEBUG"}
    filter {"configurations:release or profile"}
        defines{"NDEBUG"}

    filter "system:windows"
        defines{"_WIN32"}

    filter {"system:macosx"}
        defines{"_OSX"}

    filter {"system:linux"}
        defines{"_LINUX"}

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

print("Executing projects.lua")
include 'projects.lua'

for _, path in ipairs(PROJECTS) do
    print("Executing " .. path .. "/premake5.lua")
    include(path .. "/premake5.lua")
end

include "SolutionItems/misc.lua"