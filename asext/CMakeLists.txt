# asext 项目
project(asext)

# 定义基础源文件
set(ASEXT_SOURCES
    dllapi.cpp
    engine_api.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows 特定源文件
if(WIN32)
    list(APPEND ASEXT_SOURCES dllmain.cpp)
endif()

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
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
    # 静态链接 libgcc 和 libstdc++ 以提高兼容性
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 