-- premake5.lua

solution "chimera-server"
  configurations { "Debug", "Release" }
  location ("build/" .. _ACTION)

project "chimera-server"
  kind "ConsoleApp"
  language "C++"
  targetdir "./"
  location ("build/" .. _ACTION)

  files { "src/**.cpp" }
  includedirs { "src/**.hpp" }

  configuration { "linux" }
    buildoptions { "-std=c++11" }

  configuration { "macosx" }
    buildoptions { "-std=c++11" }

  configuration "windows"
    links { "ws2_32" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
