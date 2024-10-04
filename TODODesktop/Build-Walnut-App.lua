project "TODODesktop"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   IncludeDir["curl"] = "external/curl/include"
   IncludeDir["nlohmann"] = "external/nlohmann"
   IncludeDir["crow"] = "external/crow"
   IncludeDir["websocketpp"] = "external/websocketpp"
   IncludeDir["boost"] = "external/boost_1_82_0"

   LibDir = {}
   LibDir["curl"] = "external/curl/lib"
   LibDir["boost"] = "external/boost_1_82_0/stage/lib"

   includedirs
   {
      "../vendor/imgui",
      "../vendor/glfw/include",

      "../Walnut/Source",
      "../Walnut/Platform/GUI",

      "%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.curl}",
      "%{IncludeDir.nlohmann}",
      "%{IncludeDir.crow}",
      "%{IncludeDir.websocketpp}",
      "%{IncludeDir.boost}",
      "../../TODOShared/"
   }

   libdirs
   {
      "%{LibDir.curl}",
      "%{LibDir.boost}",
   }

   links
   {
      "Walnut",
      "libcurl.lib",
      "libboost_random-vc143-mt-x64-1_82.lib"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "WL_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "WL_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"