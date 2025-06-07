# asusermsg 项目
project(asusermsg)

# 定义源文件
set(ASUSERMSG_SOURCES
    asusermsg.cpp
    dllapi.cpp
    dllmain.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
)

# 创建动态库
add_library(asusermsg SHARED ${ASUSERMSG_SOURCES})

# 设置目标属性
set_target_properties(asusermsg PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asusermsg"
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(asusermsg PRIVATE ASUSERMSG_EXPORTS)
endif()

# 链接库
if(UNIX)
    target_link_libraries(asusermsg
        dl
        m
    )
endif() 