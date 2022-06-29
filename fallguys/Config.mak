MODNAME = fallguys
SRCFILES = dllapi.cpp engine_api.cpp engine_hook.cpp fallguys.cpp h_export.cpp meta_api.cpp physics.cpp sdk_util.cpp server_hook.cpp ../CDetour/detours.cpp ../CDetour/asm/asm.c ../CDetour/libudis86/udis86.c ../CDetour/libudis86/itab.c ../CDetour/libudis86/decode.c 
EXTRA_CFLAGS += -DPLATFORM_POSIX
EXTRA_LINK += -L../bullet3/build/src/Bullet3Collision
EXTRA_LINK += -L../bullet3/build/src/Bullet3Common
EXTRA_LINK += -L../bullet3/build/src/Bullet3Dynamics
EXTRA_LINK += -L../bullet3/build/src/Bullet3Geometry
EXTRA_LINK += -L../bullet3/build/src/BulletCollision
EXTRA_LINK += -L../bullet3/build/src/BulletDynamics
EXTRA_LINK += -L../bullet3/build/src/LinearMath
EXTRA_LINK += -Bstatic -lBullet3Collision -lBullet3Common -lBullet3Dynamics -lBullet3Geometry -lBulletCollision -lBulletDynamics -lLinearMath -Bdynamic