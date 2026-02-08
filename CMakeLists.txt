cmake_minimum_required(VERSION 3.15)
project(metamod-fallguys)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(METAMOD_64BIT "Compile as 64bit" OFF)

if(METAMOD_64BIT)
    add_definitions(-DMETAMOD_64BIT)
endif()

# Set platform target name
if(WIN32)
    if(METAMOD_64BIT)
        set(PLATFORM_TARGET "x64")
    else()
        set(PLATFORM_TARGET "x86")
    endif()
else()
    if(METAMOD_64BIT)
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

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${PLATFORM_TARGET}/$<CONFIG>)

# Note: CMAKE_BINARY_DIR is a built-in CMake variable and should not be overridden
# Use CMAKE_CURRENT_BINARY_DIR or another variable name

# Global include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/hlsdk/common
    ${CMAKE_SOURCE_DIR}/hlsdk/dlls
    ${CMAKE_SOURCE_DIR}/hlsdk/pm_shared
    ${CMAKE_SOURCE_DIR}/hlsdk/engine
    ${CMAKE_SOURCE_DIR}/metamod
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS -D_USRDLL -D_CRT_SECURE_NO_WARNINGS)

    if(METAMOD_64BIT)
        add_definitions(-D_WIN64)
    endif()
    
    # Windows compile options - support multi-config generators
    # Use generator expressions to set the runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Set debug definitions
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
    add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
    
    # Note: According to the project file, the character set is NotSet, so do not add UNICODE definitions
    
elseif(UNIX)
    add_definitions(-DPLATFORM_POSIX -DLINUX -D_LINUX)
    
    # Linux compile options
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

# Find third-party libraries
if(WIN32)
    # Third-party library paths for Windows
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
    # Third-party library paths for Linux
    set(CAPSTONE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${OBJDIR_LINUX}/include)
    set(CAPSTONE_LIBRARIES ${CMAKE_SOURCE_DIR}/thirdparty/install/capstone/${OBJDIR_LINUX}/lib/libcapstone.a)
    message("${CAPSTONE_INCLUDE_DIR}")

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

# Add subprojects
add_subdirectory(metamod)
add_subdirectory(asext)
add_subdirectory(fallguys)
add_subdirectory(ascurl)
add_subdirectory(asqcvar)
add_subdirectory(asusermsg)

# Set project dependencies
add_dependencies(fallguys asext)
add_dependencies(ascurl asext)
add_dependencies(asqcvar asext)

# Install targets
install(TARGETS metamod asext fallguys ascurl asqcvar asusermsg
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)