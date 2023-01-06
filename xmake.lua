set_project("coro")
set_version("0.0.0")
set_xmakever("2.7.4")
set_languages("c++11")

add_rules("mode.debug", "mode.release")
add_requires("boost >= 1.80", {configs = {context = true}})
add_requires("gtest >= 1.12")

target("coro")
    set_kind("static")
    add_files("src/**.cpp")
    add_includedirs("include")
    add_packages("boost")

includes("test")
