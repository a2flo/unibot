
-- vars
local win_unixenv = false
local cygwin = false
local mingw = false
local clang_libcxx = false
local gcc_compat = false
local windows_no_cmd = false
local platform = "x32"
local system_includes = ""

-- this function returns the first result of "find basepath -name filename", this is needed on some platforms to determine the include path of a library
function find_include(filename, base_path)
	if(os.is("windows") and not win_unixenv) then
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

function add_include(path)
	system_includes = system_includes.." -isystem "..path
end


-- actual premake info
solution "unibot"
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
		if(_ARGS[argc] == "--gcc") then
			gcc_compat = true
		end
		if(_ARGS[argc] == "--platform") then
			argc=argc+1
			if(_ARGS[argc] ~= nil) then
				platform = _ARGS[argc]
			end
		end
		if(_ARGS[argc] == "--windows") then
			windows_no_cmd = true
		end
		argc=argc+1
	end

	configurations { "Release", "Debug" }
	defines { "FLOOR_NET_PROTOCOL=TCP_protocol", "TCC_LIB_ONLY=1" }

	-- os specifics
	if(not os.is("windows") or win_unixenv) then
		if(not cygwin) then
			add_include("/usr/include")
		else
			add_include("/usr/include/w32api")
		end
		add_include("/usr/local/include")
		add_include("/usr/include/libxml2")
		add_include("/usr/include/libxml")
		add_include("/usr/local/include/libxml2")
		buildoptions { "-std=c++11 -Wall" }
		
		if(clang_libcxx) then
			buildoptions { "-stdlib=libc++" }
			buildoptions { "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c99-extensions -Wno-header-hygiene" }
			buildoptions { "-Wno-gnu -Wno-documentation -Wno-system-headers -Wno-global-constructors -Wno-padded -Wno-packed" }
			buildoptions { "-Wno-switch-enum -Wno-exit-time-destructors -Wno-unknown-warning-option -Wno-nested-anon-type" }
			linkoptions { "-fvisibility=default" }
			if(not win_unixenv) then
				buildoptions { "-integrated-as" }
				defines { "FLOOR_EXPORT=1" }
				linkoptions { "-stdlib=libc++" }
				if(os.is("linux")) then
					linkoptions { "-lc++abi" }
				end
			else
				-- "--allow-multiple-definition" is necessary, because gcc is still used as a linker
				-- and will always link against libstdc++ (-> multiple definitions with libc++)
				-- also note: since libc++ is linked first, libc++'s functions will be used
				linkoptions { "-stdlib=libc++ -lc++.dll -Wl,--allow-multiple-definition" }
			end
		end
		
		if(gcc_compat) then
			buildoptions { "-Wno-trigraphs -Wreturn-type -Wunused-variable -Wno-strict-aliasing" }
		end
	end
	
	if(win_unixenv) then
		defines { "WIN_UNIXENV" }
		if(cygwin) then
			defines { "CYGWIN" }
		end
		if(mingw) then
			defines { "__WINDOWS__", "MINGW" }
			add_include("/mingw/include")
			libdirs { "/usr/lib", "/usr/local/lib" }
			buildoptions { "-Wno-unknown-pragmas" }
		end
	end
	
	if(os.is("linux") or os.is("bsd") or win_unixenv) then
		add_include("/usr/include/SDL2")
		add_include("/usr/local/include/SDL2")
		add_include("/usr/include/boost")
		add_include("/usr/local/include/boost")
		
		-- find all necessary headers (in case they aren't in /usr/include)
		local include_files = { "lua.h" }
		for i = 1, #include_files do
			if((not os.isfile("/usr/include/"..include_files[i])) and
			   (not os.isfile("/usr/local/include/"..include_files[i]))) then
			   -- search in /usr/include and /usr/local/include
				local include_path = find_include(include_files[i], "/usr/include/")
				if(include_path == "") then
					include_path = find_include(include_files[i], "/usr/local/include/")
				end
				
				if(include_path ~= "") then
					add_include(path.getdirectory(include_path))
				end
			end
		end
		
		-- set system includes
		buildoptions { system_includes }
		
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
		links { lua_lib.name }
		
		links { "crypto", "ssl" }
		if(not win_unixenv) then
			libdirs { os.findlib("SDL2"), lua_lib.dir }
			buildoptions { "`sdl2-config --cflags`" }
			linkoptions { "`sdl2-config --libs`" }
		elseif(cygwin) then
			libdirs { "/lib/w32api" }
			buildoptions { "`sdl2-config --cflags | sed -E 's/-Dmain=SDL_main//g'`" }
			linkoptions { "`sdl2-config --libs | sed -E 's/(-lmingw32|-mwindows)//g'`" }
		elseif(mingw) then
			links { "pthread" }
			buildoptions { "`sdl2-config --cflags | sed -E 's/-Dmain=SDL_main//g'`" }
			if(windows_no_cmd) then
				linkoptions { "`sdl2-config --libs`" }
			else
				-- no -mwindows flag -> creates a separate cmd window + working iostream output
				linkoptions { "`sdl2-config --libs | sed -E 's/-mwindows//g'`" }
			end
		end

		if(gcc_compat) then
			if(not mingw) then
				defines { "_GLIBCXX__PTHREADS" }
			end
			defines { "_GLIBCXX_USE_NANOSLEEP", "_GLIBCXX_USE_SCHED_YIELD" }
		end
	end

	-- prefer system platform
	if(platform == "x64") then
		platforms { "x64", "x32" }
	else
		platforms { "x32", "x64" }
	end

	configuration { "x64" }
		defines { "PLATFORM_X64" }
	
	configuration { "x32" }
		defines { "PLATFORM_X86" }


project "unibot"
	-- project settings
	targetname "unibot"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	basedir "."
	targetdir "bin"
	includedirs { "/usr/include/floor", "/usr/local/include/floor", "src/" }
	
	if(not os.is("windows") or win_unixenv) then
		prebuildcommands { "./build_version.sh" }
	end

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
		links { "floor" }
		if(not os.is("windows") or win_unixenv) then
			buildoptions { "-O3 -ffast-math" }
		end

	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
		links { "floord" }
		if(not os.is("windows") or win_unixenv) then
			buildoptions { "-gdwarf-2" }
		end
