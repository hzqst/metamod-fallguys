# ascurl project
project(ascurl)

# Define core source files
set(ASCURL_SOURCES
    ascurl.cpp
    dllapi.cpp
    engine_api.cpp
    engine_hook.cpp
    h_export.cpp
    meta_api.cpp
    server_hook.cpp
)

# Windows-specific source files
if(WIN32)
    list(APPEND ASCURL_SOURCES dllmain.cpp)
endif()

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/asext/include
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include
    ${CMAKE_SOURCE_DIR}/thirdparty/curl/include/curl
    ${CMAKE_SOURCE_DIR}/thirdparty/openssl/include
)

# Create shared library
add_library(ascurl SHARED ${ASCURL_SOURCES})

# Set target properties
set_target_properties(ascurl PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ascurl"
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(ascurl PRIVATE ASCURL_EXPORTS)
endif()

# Link libraries
if(UNIX)
    target_link_libraries(ascurl
        dl
        m
    )
    # Statically link libgcc and libstdc++ for better compatibility
    set_target_properties(metamod PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif() 