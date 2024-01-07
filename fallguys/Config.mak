MODNAME = fallguys
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp fallguys.cpp h_export.cpp meta_api.cpp physics.cpp soundengine.cpp server_hook.cpp
INCLUDEDIRS+=-I$(SDKSRC)/../asext/include
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/fmod
INCLUDEDIRS+=-I$(SDKSRC)/../thirdparty/install/bullet3/$(OBJDIR_LINUX)/include/bullet
EXTRA_CFLAGS+=-DPLATFORM_POSIX
EXTRA_LINK+=-L$(SDKSRC)/../thirdparty/install/bullet3/$(OBJDIR_LINUX)/lib
EXTRA_LINK+=-Wl,--whole-archive -lBullet3Dynamics -lBullet3Collision -lBullet3Common -lBullet3Geometry -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -Wl,--no-whole-archive