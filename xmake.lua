set_xmakever("3.0.0")
set_project("metamod-fallguys")

-- 设置 C++ 标准
add_languages("cxx17")

add_rules("mode.debug", "mode.release")

if is_arch("x64") then 
    add_defines("METAMOD_64BIT")
end

includes("xmake/*.lua")

-- 全局包含目录
add_includedirs(
    "hlsdk/common",
    "hlsdk/dlls",
    "hlsdk/pm_shared",
    "hlsdk/engine",
    "metamod"
)

-- 平台特定设置
if is_plat("windows") then 
    add_defines("WIN32","_WINDOWS","_USRDLL","_CRT_SECURE_NO_WARNINGS")
    set_runtimes(is_mode("debug") and "MTd" or "MT")
    if is_arch("x64") then 
        add_defines("_WIN64")
    end
elseif is_plat("linux") then 
    add_defines("PLATFORM_POSIX","LINUX","_LINUX")
end

-- 添加子项目
includes(
    "metamod",
    "asext",
    "fallguys",
    "ascurl",
    "asqcvar",
    "asusermsg"
)
