target("asext")
    -- 定义基础源文件
    add_files(
        "dllapi.cpp",
        "engine_api.cpp",
        "h_export.cpp",
        "meta_api.cpp",
        "server_hook.cpp"
    )
    -- Windows 特定源文件
    if is_plat("windows") then
        add_files("dllmain.cpp")
    end
    -- 添加包含目录
    add_includedirs(
        "include",
        {public = true}
    )
    -- 如果需要 Bullet3
    if is_plat("linux") then
        add_packages("bullet3")
    end
    -- 创建动态库
    set_kind("shared")
    -- Windows 特定设置
    add_defines("ASEXT_EXPORTS")
    -- 静态链接 libgcc 和 libstdc++ 以提高兼容性
    if is_plat("linux") then
        set_runtimes("stdc++_static")
    end
