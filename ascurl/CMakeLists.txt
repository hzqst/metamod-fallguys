# ascurl 项目
project(ascurl)

# 定义基础源文件
set(ASCURL_SOURCES
    ascurl.cpp
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows 特定源文件
if(WIN32)
    list(APPEND ASCURL_SOURCES dllmain.cpp)
endif()

# 添加包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/curl/include/curl
    ${CMAKE_SOURCE_DIR}/thirdparty/openssl/include
)

# 创建动态库
add_library(ascurl SHARED ${ASCURL_SOURCES})

# 设置目标属性
set_target_properties(ascurl PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ascurl"
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(ascurl PRIVATE ASCURL_EXPORTS)
endif()

# 链接库
if(UNIX)
    target_link_libraries(ascurl
        dl
        m
    )
endif() 