set_project("pvz-ng")
add_rules("mode.debug", "mode.release")
set_version("0.0.1")

set_languages("c++20")

add_requires("godotcpp4")

option("enable_test")
    set_default(true)
    set_showmenu(true)
    set_description("Enable test")
option_end()

target("pvz-ng")
    set_kind("shared")
    add_packages("godotcpp4")
    add_files("cpp-src/*.cpp")
    after_build(function (target) 
        import("core.project.config")
        local target_file = target:targetfile();
        local gd_dep_file = path.join(os.projectdir(), "cpp-extension", "bin" , "lib" .. target:basename()
                                                                        .. "." .. config.get("plat")
                                                                        .. "." .. config.get("mode")
                                                                        .. "." .. os.arch()
                                                                        .. path.extension(target_file));
        os.cp(target_file, gd_dep_file)
        print(gd_dep_file)
    end)
target_end()

if has_config("enable_test") then 
    includes("tests")
end