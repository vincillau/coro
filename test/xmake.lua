target("test_spawn")
    set_kind("binary")
    set_group("test")
    add_files("spawn_test.cpp")
    add_includedirs("$(projectdir)/include")
    add_deps("coro")
    add_packages("boost", "gtest")

target("test_promise")
    set_kind("binary")
    set_group("test")
    add_files("promise_test.cpp")
    add_includedirs("$(projectdir)/include")
    add_deps("coro")
    add_packages("boost", "gtest")

target("test_sched_stack")
    set_kind("binary")
    set_group("test")
    add_files("sched/stack_test.cpp")
    add_includedirs("$(projectdir)/include")
    add_deps("coro")
    add_packages("boost", "gtest")

target("test_sched_sched")
    set_kind("binary")
    set_group("test")
    add_files("sched/sched_test.cpp")
    add_includedirs("$(projectdir)/include")
    add_deps("coro")
    add_packages("boost", "gtest")

target("test_sched_promise")
    set_kind("binary")
    set_group("test")
    add_files("sched/promise_test.cpp")
    add_includedirs("$(projectdir)/include")
    add_deps("coro")
    add_packages("boost", "gtest")