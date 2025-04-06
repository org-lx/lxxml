workspace "lxxml"
   architecture "x86_64"
   configurations { "Debug", "Release"}

project "lxxml"
   kind "StaticLib"
   language "C"
   cdialect "c99"
   targetdir "bin"
   includedirs { "include" }
   files { "src/**.c", "include/**.h" }
   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "test-app"
	kind "ConsoleApp"
	language "C"
   cdialect "c99"
   targetdir "bin"
   includedirs { "include" }
   files { "example/**.c", "example/**.h" }
	
	links { "lxxml" }
   
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
