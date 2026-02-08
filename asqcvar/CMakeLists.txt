# asqcvar project
project(asqcvar)

# Define core source files
set(ASQCVAR_SOURCES
    asqcvar.cpp
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows-specific source files
if(WIN32)
    list(APPEND ASQCVAR_SOURCES dllmain.cpp)
endif()


# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
)

# Create shared library
add_library(asqcvar SHARED ${ASQCVAR_SOURCES})

# Set target properties
set_target_properties(asqcvar PROPERTIES
    PREFIX ""
    OUTPUT_NAME "asqcvar"
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(asqcvar PRIVATE ASQCVAR_EXPORTS)
endif()

# Link libraries
if(UNIX)
    target_link_libraries(asqcvar
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 