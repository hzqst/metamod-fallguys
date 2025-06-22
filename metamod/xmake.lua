target("metamod")
    -- 定义源文件
    add_files(
        "api_hook.cpp",
        "api_info.cpp",
        "CDetour/asm/asm.cpp",
        "CDetour/detours.cpp",
        "commands_meta.cpp",
        "conf_meta.cpp",
        "dllapi.cpp",
        "engineinfo.cpp",
        "engine_api.cpp",
        "game_autodetect.cpp",
        "game_support.cpp",
        "h_export.cpp",
        "linkgame.cpp",
        "linkplug.cpp",
        "log_meta.cpp",
        "metamod.cpp",
        "meta_eiface.cpp",
        "mhook.cpp",
        "mlist.cpp",
        "mplayer.cpp",
        "mplugin.cpp",
        "mqueue.cpp",
        "mreg.cpp",
        "mutil.cpp",
        "osdep.cpp",
        "osdep_p.cpp",
        "reg_support.cpp",
        "sdk_util.cpp",
        "studioapi.cpp",
        "support_meta.cpp",
        "thread_logparse.cpp",
        "vdate.cpp"
    )
    -- 平台特定的源文件
    if is_plat("windows") then
        add_files("osdep_detect_gamedll_win32.cpp", "osdep_linkent_win32.cpp")
    else
        add_files("osdep_detect_gamedll_linux.cpp", "osdep_linkent_linux.cpp")
    end
    -- 添加包含目录
    add_includedirs("CDetour")
    -- Linux 特定的包含目录
    if is_plat("linux") then 
        add_packages("procmap")
    end
    -- 添加预处理器定义
    add_defines("__METAMOD_BUILD__")
    -- 创建动态库
    set_kind("shared")
    -- Windows 特定设置
    if is_plat("windows") then 
        add_files("metamod.def", "res_meta.rc")
    end
    -- 链接库
    add_packages("capstone")
    -- 静态链接 libgcc 和 libstdc++ 以提高兼容性
    if is_plat("linux") then
        set_runtimes("stdc++_static")
    end
