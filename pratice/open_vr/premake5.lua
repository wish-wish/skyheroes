workspace "openvr"
   configurations { "Debug", "Release" }

project "openvr_api"
    kind        "StaticLib"
    language    "C++"
    warnings    "off"
    
    includedirs { "src" }
    includedirs { "jit" }
    includedirs { "host" }
    includedirs { "dynasm" }
    
    --links { '' }
    
    files 
    { 
        "**.h", 
        "**.cpp" 
    }
    
    excludes
    {
        --"src/lauxlib.c",
        --"src/lua.c",
        --"src/luac.c",
        --"src/print.c",
        --"**.lua",
        --"src/host/minilua.c",
        --"src/lib_*.c",
        --"src/luajit.c",
        --"src/ljamalg.c",
        --"etc/*.c"
    }
    
   filter { "configurations:Debug" }
      defines { "DEBUG" }
      flags { "Symbols" }

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"

