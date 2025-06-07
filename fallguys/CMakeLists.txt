# fallguys 项目
project(fallguys)

# 定义基础源文件
set(FALLGUYS_SOURCES
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    fallguys.cpp
    h_export.cpp
    meta_api.cpp
    physics.cpp
    server_hook.cpp
    soundengine.cpp
)

# Windows 特定源文件
if(WIN32)
    list(APPEND FALLGUYS_SOURCES dllmain.cpp)
endif()

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/fmod
    ${BULLET3_INCLUDE_DIR}
    ${CAPSTONE_INCLUDE_DIR}
)

# 创建动态库
add_library(fallguys SHARED ${FALLGUYS_SOURCES})

# 设置目标属性
set_target_properties(fallguys PROPERTIES
    PREFIX ""
    OUTPUT_NAME "fallguys"
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(fallguys PRIVATE FALLGUYS_EXPORTS)
endif()

# 链接库
target_link_libraries(fallguys
    ${BULLET3_LIBRARIES}
)

# Linux 特定链接
if(UNIX)
    target_link_libraries(fallguys
        dl
        m
    )
    # 静态链接 libgcc 和 libstdc++ 以提高兼容性
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 