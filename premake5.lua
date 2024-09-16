-- premake5.lua
workspace "TODODesktop"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TODODesktop"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "TODODesktop"