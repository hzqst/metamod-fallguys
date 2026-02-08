# asext project
project(asext)

# Define core source files
set(ASEXT_SOURCES
    dllapi.cpp
    engine_api.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows-specific source files
if(WIN32)
    list(APPEND ASEXT_SOURCES dllmain.cpp)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
)

# If Bullet3 is needed
if(UNIX)
    include_directories(${BULLET3_INCLUDE_DIR})
endif()

# Create shared library
add_library(asext SHARED ${ASEXT_SOURCES})

# Set target properties
set_target_properties(asext PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asext"
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(asext PRIVATE ASEXT_EXPORTS)
endif()

# Link libraries
if(UNIX)
    target_link_libraries(asext
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 