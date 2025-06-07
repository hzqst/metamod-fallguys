# asext 项目
project(asext)

# 定义源文件
set(ASEXT_SOURCES
    dllapi.cpp
    dllmain.cpp
    engine_api.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
)

# 如果需要 Bullet3
if(UNIX)
    include_directories(${BULLET3_INCLUDE_DIR})
endif()

# 创建动态库
add_library(asext SHARED ${ASEXT_SOURCES})

# 设置目标属性
set_target_properties(asext PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asext"
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(asext PRIVATE ASEXT_EXPORTS)
endif()

# 链接库
if(UNIX)
    target_link_libraries(asext
        dl
        m
    )
endif() 