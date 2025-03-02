project "cereal"
    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++20"
    staticruntime "on"
    systemversion "latest"
    warnings      "extra"
    pchheader     "pch.h"
    pchsource     "src/pch.cpp"
    targetdir     (BIN_DIR)
    objdir        (OBJ_DIR)

    
    vpaths 
    {
        ["Header Files/*"] = { "include/**.h",  "include/**.hpp", "src/**.h", "src/**.hpp"},
        ["Source Files/*"] = {"src/**.c", "src/**.cpp"},
    }

    files {"src/**.c", "src/**.cpp", "src/**.h", "src/**.hpp", "include/**.h", "include/**.hpp"}

    includedirs {
        "include/cereal",
        "src/"
    }
    links {
        
    }
    defines {

    }