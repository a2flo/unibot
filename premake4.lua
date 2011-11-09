
-- vars
local cygwin = false
local clang_libcxx = false

-- this function returns the first result of "find basepath -name filename", this is needed on some platforms to determine the include path of a library
function find_include(filename, base_path)
	if(os.is("windows") and not cygwin) then
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
	configurations { "Release", "Debug" }

project "unibot"
	targetname "unibot"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.h", "src/**.cpp" }
	platforms { "x64", "x32" }
	
	-- scan args
	local argc = 1
	while(_ARGS[argc] ~= nil) do
		if(_ARGS[argc] == "--env") then
			argc=argc+1
			-- check if we are building with cygwin/mingw
			if(_ARGS[argc] ~= nil and _ARGS[argc] == "cygwin") then
				cygwin = true
				win_unixenv = true
			end
			if(_ARGS[argc] ~= nil and _ARGS[argc] == "mingw") then
				mingw = true
				win_unixenv = true
			end
		end
		if(_ARGS[argc] == "--clang") then
			clang_libcxx = true
		end
		argc=argc+1
	end

	-- os specifics
	if(not os.is("windows") or cygwin) then
		includedirs { "/usr/include", "/usr/local/include", "src/", "src/threading/" }
		buildoptions { "-Wall -x c++ -std=c++0x -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops" }
		buildoptions { "-msse3" }
		prebuildcommands { "./build_version.sh" }
		defines { "UNIBOT_NET_PROTOCOL=TCP_protocol" }
		if(clang_libcxx) then
			buildoptions { "-stdlib=libc++" }
			buildoptions { "-Wno-delete-non-virtual-dtor -Wno-overloaded-virtual" }
			linkoptions { "-stdlib=libc++ -fvisibility=default" }
		end
	end
	
	if(cygwin) then
		-- only works with gnu++0x for now ...
		buildoptions { "-std=gnu++0x" }
		defines { "CYGWIN" }
	end
	
	if(os.is("linux") or os.is("bsd") or cygwin) then
		-- find lua lib (try different lib names)
		local lua_lib_names = { "lua", "lua5.1", "lua-5.1", "lua5.2", "lua-5.2" }
		local lua_lib = { name = nil, dir = nil }
		for i = 1, #lua_lib_names do
			lua_lib.name = lua_lib_names[i]
			lua_lib.dir = os.findlib(lua_lib.name)
			if(lua_lib.dir ~= nil) then
				break
			end
		end
		
		links { "SDL_net", lua_lib.name }
		if(not cygwin) then
			libdirs { os.findlib("SDL"), os.findlib("SDL_net"), lua_lib.dir }
			buildoptions { "$(sdl-config --cflags)" }
			linkoptions { "$(sdl-config --libs)" }
		else
			buildoptions { "`sdl-config --cflags | sed -E 's/-Dmain=SDL_main//g'`" }
			linkoptions { "`sdl-config --libs | sed -E 's/(-lmingw32|-mwindows)//g'`" }
		end
		defines { "_GLIBCXX__PTHREADS", "_GLIBCXX_USE_NANOSLEEP" }
		
		-- find all necessary headers (in case they aren't in /usr/include)
		local include_files = { "SDL.h", "SDL_net.h", "lua.h" }
		for i = 1, #include_files do
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
		buildoptions { "-Iinclude -I/usr/local/include -isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6 -gdwarf-2 -mdynamic-no-pic" }
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
