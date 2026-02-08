# fallguys project
project(fallguys)

# Define core source files
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

# Windows-specific source files
if(WIN32)
    list(APPEND FALLGUYS_SOURCES dllmain.cpp)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
    ${CMAKE_SOURCE_DIR}/thirdparty/fmod
    ${BULLET3_INCLUDE_DIR}
    ${CAPSTONE_INCLUDE_DIR}
)

# Create shared library
add_library(fallguys SHARED ${FALLGUYS_SOURCES})

# Set target properties
set_target_properties(fallguys PROPERTIES
    PREFIX ""
    OUTPUT_NAME "fallguys"
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(fallguys PRIVATE FALLGUYS_EXPORTS)
endif()

# Link libraries
target_link_libraries(fallguys
    ${BULLET3_LIBRARIES}
)

# Linux-specific linking
if(UNIX)
    target_link_libraries(fallguys
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 