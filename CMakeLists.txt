cmake_minimum_required(VERSION 3.15)
project(metamod-fallguys)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置平台名称
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_TARGET "x64")
    else()
        set(PLATFORM_TARGET "x86")
    endif()
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(PLATFORM_TARGET "dbg.linux_amd64")
            set(OBJDIR_LINUX "dbg.linux_amd64")
        else()
            set(PLATFORM_TARGET "opt.linux_amd64")
            set(OBJDIR_LINUX "opt.linux_amd64")
        endif()
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(PLATFORM_TARGET "dbg.linux_i386")
            set(OBJDIR_LINUX "dbg.linux_i386")
        else()
            set(PLATFORM_TARGET "opt.linux_i386")
            set(OBJDIR_LINUX "opt.linux_i386")
        endif()
    endif()
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)

# 注意：CMAKE_BINARY_DIR 是 CMake 内置变量，不应该被覆盖
# 使用 CMAKE_CURRENT_BINARY_DIR 或其他变量名

# 全局包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/hlsdk/common
    ${CMAKE_SOURCE_DIR}/hlsdk/dlls
    ${CMAKE_SOURCE_DIR}/hlsdk/pm_shared
    ${CMAKE_SOURCE_DIR}/hlsdk/engine
    ${CMAKE_SOURCE_DIR}/metamod
)

# 平台特定设置
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS -D_USRDLL -D_CRT_SECURE_NO_WARNINGS)
    
    # Windows 下的编译选项 - 支持多配置生成器
    # 使用生成器表达式来设置运行时库
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # 设置调试信息
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
    add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
    
    # 注意：根据项目文件，字符集设置为 NotSet，所以不添加 UNICODE 定义
    
elseif(UNIX)
    add_definitions(-DPLATFORM_POSIX -DLINUX -D_LINUX)
    
    # Linux 下的编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -fPIC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    else()
        add_definitions(-DNDEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
endif()

# 查找第三方库
if(WIN32)
    # Windows 下的第三方库路径
    set(CAPSTONE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${PLATFORM_TARGET}/Release/include)
    set(CAPSTONE_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${PLATFORM_TARGET}/Release/lib/capstone.lib)
    
    set(BULLET3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/include/bullet)
    set(BULLET3_LIBRARIES 
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/Bullet3Dynamics.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/Bullet3Collision.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/Bullet3Common.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/Bullet3Geometry.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/BulletSoftBody.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/BulletDynamics.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/BulletCollision.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${PLATFORM_TARGET}/Release/lib/LinearMath.lib
    )
else()
    # Linux 下的第三方库路径
    set(CAPSTONE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${OBJDIR_LINUX}/include)
    set(CAPSTONE_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${OBJDIR_LINUX}/lib/libcapstone.a)
    
    set(BULLET3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/include/bullet)
    set(BULLET3_LIBRARIES 
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBullet3Dynamics.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBullet3Collision.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBullet3Common.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBullet3Geometry.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBulletSoftBody.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBulletDynamics.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libBulletCollision.a
        ${CMAKE_SOURCE_DIR}/thirdparty/install/bullet3/${OBJDIR_LINUX}/lib/libLinearMath.a
    )
    
    set(PROCMAP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/procmap/${OBJDIR_LINUX}/include)
    set(PROCMAP_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/install/procmap/${OBJDIR_LINUX}/lib/libprocmap.a)
endif()

# 添加子项目
add_subdirectory(metamod)
add_subdirectory(asext)
add_subdirectory(fallguys)
add_subdirectory(ascurl)
add_subdirectory(asqcvar)
add_subdirectory(asusermsg)

# 设置项目依赖关系
add_dependencies(fallguys asext)
add_dependencies(ascurl asext)
add_dependencies(asqcvar asext)

# 安装目标
install(TARGETS metamod asext fallguys ascurl asqcvar asusermsg
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)