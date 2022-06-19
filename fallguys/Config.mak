MODNAME = fallguys
SRCFILES = fallguys.cpp engine_hook.cpp server_hook.cpp dllapi.cpp engine_api.cpp h_export.cpp meta_api.cpp sdk_util.cpp ../CDetour/detours.cpp ../CDetour/asm/asm.c ../CDetour/libudis86/udis86.c ../CDetour/libudis86/itab.c ../CDetour/libudis86/decode.c 
EXTRA_CFLAGS += -DPLATFORM_POSIX
