-- Define the project
set_project("fast-mrmr")
set_version("0.1.0", {build = "%Y%m%d%H%M"})

-- Set the languages and toolchains
set_languages("cxx23")
set_toolchains("clang")
set_defaultplat("windows")
set_defaultarchs("x64")

-- Auto-update the compile_commands.json file
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- Set the default build mode to release
add_rules("mode.debug", "mode.release")
set_default("mode", "release")

-- Set warnings
set_warnings("allextra", "error")

-- Define the fast-mrmr_core core library
target("fast-mrmr_core")
    set_kind("static")
    add_files("src/**.cpp")
    add_includedirs("src", {public = true})

-- Define the fast-mrmr_cli application target
target("fast-mrmr_cli")
    set_kind("binary")
    add_deps("fast-mrmr_core")
    add_files("apps/fast-mrmr_cli.cpp")

target("data_reader")
    set_kind("binary")
    add_files("apps/data_reader.cpp")
    add_deps("fast-mrmr_core")