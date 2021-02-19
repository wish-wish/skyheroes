workspace "lua"
   configurations { "Debug", "Release" }

project "lua51"
    kind        "StaticLib"
    language    "C"
    warnings    "off"
    
    files 
    { 
        "**.h", 
        "**.c" 
    }
    
    excludes
    {
        --"src/lauxlib.c",
        --"src/lua.c",
        --"src/luac.c",
        --"src/print.c",
        "**.lua",
        "etc/*.c"
    }
    
   filter { "configurations:Debug" }
      defines { "DEBUG" }
      flags { "Symbols" }

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"

