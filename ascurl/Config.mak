MODNAME = ascurl
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp ascurl.cpp h_export.cpp meta_api.cpp server_hook.cpp
INCLUDEDIRS+=-I$(SDKSRC)/../asext/include
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/curl/include/curl
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/openssl/include
EXTRA_CFLAGS+=-DPLATFORM_POSIX