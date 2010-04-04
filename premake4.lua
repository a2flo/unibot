
-- this function returns the first result of "find basepath -name filename", this is needed on some platforms to determine the include path of a library
function find_include(filename, base_path)
	if(os.is("windows")) then
		return ""
	end
	
	local proc = io.popen("find "..base_path.." -name \""..filename.."\"", "r")
	local path_names = proc:read("*a")
	proc:close()
	
	if(string.len(path_names) == 0) then
		return ""
	end
	
	local newline = string.find(path_names, "\n")
	if newline == nil then
		return ""
	end
	
	return string.sub(path_names, 0, newline-1)
end


-- actual premake info
solution "unibot"
	configurations { "Debug", "Release" }

project "unibot"
	targetname "unibot"
	kind "ConsoleApp"
	language "C++"
	files { "*.h", "*.cpp" }
	platforms { "x32", "x64" }

	if(not os.is("windows")) then
		includedirs { "/usr/include", "/usr/local/include" }
		buildoptions { "-Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize" }
		buildoptions { "-msse3 -fvisibility=hidden -fvisibility-inlines-hidden" }
		prebuildcommands { "./build_version.sh" }
	end
	
	if(os.is("linux") or os.is("bsd")) then
		-- find lua lib (try different lib names)
		local lua_lib_names = { "lua", "lua5.1", "lua-5.1" }
		local lua_lib = { name = nil, dir = nil }
		for i = 1, table.maxn(lua_lib_names) do
			lua_lib.name = lua_lib_names[i]
			lua_lib.dir = os.findlib(lua_lib.name)
			if(lua_lib.dir ~= nil) then
				break
			end
		end
		
		libdirs { os.findlib("SDL"), os.findlib("SDL_net"), lua_lib.dir }
		links { "SDL", "SDLmain", "SDL_net", lua_lib.name }
		buildoptions { "`sdl-config --cflags`" }
		linkoptions { "`sdl-config --libs`" }
		defines { "_GLIBCXX__PTHREADS" }
		
		-- find all necessary headers (in case they aren't in /usr/include)
		local include_files = { "SDL.h", "SDL_net.h", "lua.h" }
		for i = 1, table.maxn(include_files) do
			if((not os.isfile("/usr/include/"..include_files[i])) and
			   (not os.isfile("/usr/local/include/"..include_files[i]))) then
			   -- search in /usr/include and /usr/local/include
				local include_path = find_include(include_files[i], "/usr/include/")
				if(include_path == "") then
					include_path = find_include(include_files[i], "/usr/local/include/")
				end
				
				if(include_path ~= "") then
					includedirs { path.getdirectory(include_path) }
				end
			end
		end
	end
	
	if(os.is("macosx")) then
		buildoptions { "-Iinclude -I/usr/local/include -isysroot /Developer/SDKs/MacOSX10.6.sdk -msse4.1 -mmacosx-version-min=10.6 -gdwarf-2 -mdynamic-no-pic" }
		linkoptions { "-isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6 -framework SDL_net -framework SDL -framework lua -framework Cocoa -framework AppKit -framework Foundation" }
	end
	
	configuration { "x32" }
		defines { "PLATFORM_X86" }
	
	configuration { "x64" }
		defines { "PLATFORM_X64" }

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		buildoptions { "-O3" }
