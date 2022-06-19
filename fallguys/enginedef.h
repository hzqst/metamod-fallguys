#pragma once

#include "signatures.h"

#ifdef PLATFORM_WINDOWS

#define ENGINE_DLL_NAME "hw.dll"

#else

#define ENGINE_DLL_NAME "hw.so"

#endif

//Engine private functions

// SV_PushEntity
typedef trace_t *(__cdecl *fnSV_PushEntity)(trace_t * trace, edict_t *ent, float * push);
trace_t* NewSV_PushEntity(trace_t* trace, edict_t* ent, float* push);
PRIVATE_FUNCTION_EXTERN(SV_PushEntity);

// SV_PushMove
typedef void(*fnSV_PushMove)(edict_t *pusher, float movetime);
void NewSV_PushMove(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushMove);

// SV_PushRotate
typedef void(*fnSV_PushRotate)(edict_t *pusher, float movetime);
void NewSV_PushRotate(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushRotate);