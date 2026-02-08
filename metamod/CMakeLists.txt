# metamod project
project(metamod)

# Define source files
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

# Platform-specific source files
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

# Add include directories
include_directories(
    CDetour
    ${CAPSTONE_INCLUDE_DIR}
)

# Linux-specific include directories
if(UNIX)
    include_directories(${PROCMAP_INCLUDE_DIR})
endif()

# Add preprocessor definitions
add_definitions(-D__METAMOD_BUILD__)

# Create shared library
add_library(metamod SHARED ${METAMOD_SOURCES})

# Set target properties
set_target_properties(metamod PROPERTIES
    PREFIX ""
    OUTPUT_NAME "metamod"
)

# Windows-specific settings
if(WIN32)
    # Add module definition file
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/metamod.def\""
    )
    
    # Add resource file
    target_sources(metamod PRIVATE res_meta.rc)
endif()

# Link libraries
target_link_libraries(metamod
    ${CAPSTONE_LIBRARIES}
)

# Linux-specific linking
if(UNIX)
    target_link_libraries(metamod
        ${PROCMAP_LIBRARIES}
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 