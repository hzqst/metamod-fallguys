MODNAME = fallguys
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp fallguys.cpp h_export.cpp meta_api.cpp physics.cpp sdk_util.cpp server_hook.cpp ../CDetour/detours.cpp ../CDetour/asm/asm.c ../CDetour/libudis86/udis86.c ../CDetour/libudis86/itab.c ../CDetour/libudis86/decode.c 
INCLUDEDIRS+=-I$(SDKSRC)/../CDetour -I$(SDKSRC)/../libudis86 -I$(SDKSRC)/../bullet3/build/include/bullet
EXTRA_CFLAGS += -DPLATFORM_POSIX