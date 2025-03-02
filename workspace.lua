workspace (WORKSPACE_NAME)
    print("Setting start project to " .. START_PROJECT)
    startproject (START_PROJECT)
    configurations {
        "debug",
        "profile",
        "release"
    }
    platforms {
        "x64"
        -- "x86_64",
        -- "ARM64"
    }

    defaultplatform("x64")

    filter "configurations:debug"
        runtime  "debug"
        symbols  "on"
        optimize "off"
    filter "configurations:profile"
        runtime  "release"
        symbols  "on"
        optimize "on"
    filter "configurations:release"
        runtime  "release"
        symbols  "off"
        optimize "on"
    filter "platforms:x86_64"
        architecture "x86_64"
    filter "platforms:x64"
        architecture "x64"
    filter { "platforms:Arm64" }
        architecture "ARM64"

    filter {}