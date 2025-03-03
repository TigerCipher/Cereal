project "SolutionItems"
    kind "Utility"
    targetdir     (BIN_DIR)
    objdir        (OBJ_DIR)

    files {
        ROOT_PATH "README.md",
        ROOT_PATH "LICENSE",
        ROOT_PATH ".gitignore",
        ROOT_PATH ".gitmodules",
        ROOT_PATH "*.lua",
        PROJ_DIR .. "/*/premake5.lua",
        DEP_DIR .. "/*/premake5.lua",
        ROOT_PATH "docs/**.md",
        ROOT_PATH ".clang-format",
    }