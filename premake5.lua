-- premake5.lua

-- Global Variables
LOCATION_DIRECTORY_NAME = "build"

-- Workspace
workspace "BeeMP"
	configurations
	{
		"Debug",
		"Release"
	}
	system "Windows"
	architecture "x86_64"
	project "BeeMP_Sample"
	location(LOCATION_DIRECTORY_NAME)

project "BeeMP_Sample"
	kind "ConsoleApp"
	language "C++"
	files
	{
		"beemp.h", "main.cpp"
	}
	location(LOCATION_DIRECTORY_NAME)
    targetdir "bin/%{cfg.buildcfg}"