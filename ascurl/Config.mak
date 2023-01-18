MODNAME = ascurl
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp ascurl.cpp h_export.cpp meta_api.cpp server_hook.cpp
INCLUDEDIRS+=-I$(SDKSRC)/../curl/include/curl
INCLUDEDIRS+=-I$(SDKSRC)/../openssl/include
INCLUDEDIRS+=-I$(SDKSRC)/../asext/include
EXTRA_CFLAGS+=-DPLATFORM_POSIX