# asusermsg project
project(asusermsg)

# Define core source files
set(ASUSERMSG_SOURCES
    asusermsg.cpp
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows-specific source files
if(WIN32)
    list(APPEND ASUSERMSG_SOURCES dllmain.cpp)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
)

# Create shared library
add_library(asusermsg SHARED ${ASUSERMSG_SOURCES})

# Set target properties
set_target_properties(asusermsg PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asusermsg"
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(asusermsg PRIVATE ASUSERMSG_EXPORTS)
endif()

# Link libraries
if(UNIX)
    target_link_libraries(asusermsg
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 