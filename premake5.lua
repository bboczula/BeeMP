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

project "BeeMP_Sample"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}/bin"
	files
	{
		"beemp.h", "main.cpp"
	}