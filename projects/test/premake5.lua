project "Test"
    kind          "ConsoleApp"
    language      "C++"
    cppdialect    "C++20"
    staticruntime "on"
    systemversion "latest"
    warnings      "extra"
    targetdir     (BIN_DIR)
    objdir        (OBJ_DIR)


    filter "action:vs*"
            debugdir "$(SolutionDir)"

    filter {}

    vpaths 
    {
        ["Header Files/*"] = { "include/**.h",  "include/**.hpp", "src/**.h", "src/**.hpp"},
        ["Source Files/*"] = {"src/**.c", "src/**.cpp"},
    }

    files {"src/**.c", "src/**.cpp", "src/**.h", "src/**.hpp", "include/**.h", "include/**.hpp"}

    includedirs {
        "src",
        PROJ_DIR .. "/cereal/include",
    }

    links {
        "cereal"
    }

    platform_defines()


    filter "action:vs*"
        characterset ("Unicode")
        buildoptions { "/Zc:__cplusplus" }

    filter{}