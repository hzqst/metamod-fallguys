MODNAME = metamod

#__METAMOD_BUILD__ for our special eiface.h
EXTRA_CFLAGS += -D__METAMOD_BUILD__ 
EXTRA_CFLAGS += -DPLATFORM_POSIX
EXTRA_LINK2 += -ldl -static-libgcc -static-libstdc++
#-DMETA_PERFMON

SRCFILES = api_hook.cpp api_info.cpp commands_meta.cpp conf_meta.cpp \
	dllapi.cpp engine_api.cpp engineinfo.cpp game_support.cpp \
	game_autodetect.cpp h_export.cpp linkgame.cpp linkplug.cpp \
	log_meta.cpp meta_eiface.cpp metamod.cpp mlist.cpp mplayer.cpp \
	mplugin.cpp mqueue.cpp mreg.cpp mutil.cpp osdep.cpp \
	osdep_p.cpp reg_support.cpp sdk_util.cpp studioapi.cpp \
	support_meta.cpp thread_logparse.cpp vdate.cpp CDetour/detours.cpp CDetour/asm/asm.cpp

INFOFILES = info_name.h vers_meta.h
RESFILE = res_meta.rc

ifeq "$(OS)" "linux"
	SRCFILES+=osdep_linkent_linux.cpp osdep_detect_gamedll_linux.cpp
	EXTRA_LINK+=
else
	SRCFILES+=osdep_linkent_win32.cpp osdep_detect_gamedll_win32.cpp
	EXTRA_LINK+=-Xlinker --script -Xlinker i386pe.merge
endif

ifeq "$(OPT)" "opt"
	EXTRA_CFLAGS += -D__INTERNALS_USE_REGPARAMS__
endif

ifeq "$(OPT)" "opt-fast"
	EXTRA_CFLAGS += -D__INTERNALS_USE_REGPARAMS__
endif

INCLUDEDIRS+=-I$(SRCDIR)/CDetour
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/install/procmap/$(OBJDIR_LINUX)/include
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/install/capstone/$(OBJDIR_LINUX)/include
INCLUDEDIRS+=-include $(GLIBCHEADER)
EXTRA_LINK+=-L$(SDKSRC)/../thirdparty/install/procmap/$(OBJDIR_LINUX)/lib
EXTRA_LINK+=-L$(SDKSRC)/../thirdparty/install/capstone/$(OBJDIR_LINUX)/lib
EXTRA_LINK+=-Wl,--whole-archive -lcapstone -lprocmap -Wl,--no-whole-archive