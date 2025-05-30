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

add_requires("vcpkg::arrow[parquet,csv]", {configs = {shared = true}, alias = "arrow"})

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
    add_packages("arrow")

target("csv_to_parquet")
    set_kind("binary")
    add_files("apps/csv_to_parquet.cpp")
    add_packages("arrow")