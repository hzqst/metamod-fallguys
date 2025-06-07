# asqcvar 项目
project(asqcvar)

# 定义基础源文件
set(ASQCVAR_SOURCES
    asqcvar.cpp
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows 特定源文件
if(WIN32)
    list(APPEND ASQCVAR_SOURCES dllmain.cpp)
endif()


# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
)

# 创建动态库
add_library(asqcvar SHARED ${ASQCVAR_SOURCES})

# 设置目标属性
set_target_properties(asqcvar PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asqcvar"
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(asqcvar PRIVATE ASQCVAR_EXPORTS)
endif()

# 链接库
if(UNIX)
    target_link_libraries(asqcvar
        dl
        m
    )
    # 静态链接 libgcc 和 libstdc++ 以提高兼容性
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 