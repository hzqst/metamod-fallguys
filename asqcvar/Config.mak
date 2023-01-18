MODNAME = asqcvar
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp asqcvar.cpp h_export.cpp meta_api.cpp server_hook.cpp
INCLUDEDIRS+=-I$(SDKSRC)/../asext/include
EXTRA_CFLAGS+=-DPLATFORM_POSIX