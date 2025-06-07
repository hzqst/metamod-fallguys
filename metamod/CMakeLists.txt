# metamod 项目
project(metamod)

# 定义源文件
set(METAMOD_SOURCES
    api_hook.cpp
    api_info.cpp
    CDetour/asm/asm.cpp
    CDetour/detours.cpp
    commands_meta.cpp
    conf_meta.cpp
    dllapi.cpp
    engineinfo.cpp
    engine_api.cpp
    game_autodetect.cpp
    game_support.cpp
    h_export.cpp
    linkgame.cpp
    linkplug.cpp
    log_meta.cpp
    metamod.cpp
    meta_eiface.cpp
    mhook.cpp
    mlist.cpp
    mplayer.cpp
    mplugin.cpp
    mqueue.cpp
    mreg.cpp
    mutil.cpp
    osdep.cpp
    osdep_p.cpp
    reg_support.cpp
    sdk_util.cpp
    studioapi.cpp
    support_meta.cpp
    thread_logparse.cpp
    vdate.cpp
)

# 平台特定的源文件
if(WIN32)
    list(APPEND METAMOD_SOURCES
        osdep_detect_gamedll_win32.cpp
        osdep_linkent_win32.cpp
    )
else()
    list(APPEND METAMOD_SOURCES
        osdep_detect_gamedll_linux.cpp
        osdep_linkent_linux.cpp
    )
endif()

# 添加包含目录
include_directories(
    CDetour
    ${CAPSTONE_INCLUDE_DIR}
)

# Linux 特定的包含目录
if(UNIX)
    include_directories(${PROCMAP_INCLUDE_DIR})
endif()

# 添加预处理器定义
add_definitions(-D__METAMOD_BUILD__)

# 创建动态库
add_library(metamod SHARED ${METAMOD_SOURCES})

# 设置目标属性
set_target_properties(metamod PROPERTIES
    PREFIX ""
    OUTPUT_NAME "metamod"
)

# Windows 特定设置
if(WIN32)
    # 添加模块定义文件
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/metamod.def\""
    )
    
    # 添加资源文件
    target_sources(metamod PRIVATE res_meta.rc)
endif()

# 链接库
target_link_libraries(metamod
    ${CAPSTONE_LIBRARIES}
)

# Linux 特定链接
if(UNIX)
    target_link_libraries(metamod
        ${PROCMAP_LIBRARIES}
        dl
        m
    )
endif() 