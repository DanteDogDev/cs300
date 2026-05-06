set_defaultmode("debug")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

add_requires("libsdl3", "glew", "glm")

set_languages("c++17")

if is_mode("debug") then
	set_optimize("none")
	add_cxxflags("-Og")
end

target("cs300", function()
    set_kind("binary")
    
    add_packages("libsdl3", "glew", "glm")
    
    add_files("src/**.cpp")
    add_includedirs("src")
    set_rundir("$(projectdir)")

    -- if is_plat("windows") then
    --     add_syslinks("opengl32")
    -- elseif is_plat("linux") then
    --     add_syslinks("GL")
    -- end
end)
