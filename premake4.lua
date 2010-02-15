
solution "unibot"
	configurations { "Debug", "Release" }

project "unibot"
	targetname "unibot"
	kind "ConsoleApp"
	language "C++"
	files { "*.h", "*.cpp" }
	platforms { "native", "x32", "x64" }

	if(not os.is("windows")) then
		includedirs { "/usr/include", "/usr/local/include" }
		buildoptions { "-Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize" }
		buildoptions { "-msse3 -fvisibility=hidden -fvisibility-inlines-hidden -fopenmp" }
		linkoptions { "-fopenmp" }
	end
	
	if(os.is("linux") or os.is("bsd")) then
		libdirs { os.findlib("SDL"), os.findlib("SDL_net"), os.findlib("lua") }
		links { "SDL", "SDLmain", "SDL_net", "lua" }
		buildoptions { "`sdl-config --cflags`" }
		linkoptions { "`sdl-config --libs`" }
	end
	
	if(os.is("macosx")) then
		files { "osx/**.h", "osx/**.cpp", "osx/**.m", "osx/**.mm" }
		buildoptions { "-Iinclude -I/usr/local/include -isysroot /Developer/SDKs/MacOSX10.6.sdk -msse4.1 -mmacosx-version-min=10.6 -gdwarf-2 -fpascal-strings -fasm-blocks -mdynamic-no-pic" }
		linkoptions { "-isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6 -framework SDL_net -framework SDL -framework Cocoa -framework AppKit -framework Foundation -framework lua" }
	end

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		buildoptions { "-O3" }
