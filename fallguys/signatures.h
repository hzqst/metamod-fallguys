#pragma once

#include "util.h"
#include "detours.h"

#define PRIVATE_FUNCTION_DEFINE(name) CDetour *g_hook_##name;\
fn##name g_pfn_##name;\
fn##name g_call_original_##name;

#define PRIVATE_FUNCTION_EXTERN(name) extern CDetour *g_hook_##name;\
extern fn##name g_pfn_##name;\
extern fn##name g_call_original_##name;

#define GetCallAddress(addr) ((PUCHAR)addr + *(int *)((PUCHAR)addr + 1) + 5)

#ifdef PLATFORM_WINDOWS

#define LOCATE_FROM_SIGNATURE(dll, sig) MH_SearchPattern(dll, MH_GetModuleSize(dll), sig, sizeof(sig) - 1);

#define SV_PushEntity_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x8B\x84\x24\x2A\x00\x00\x00"
#define SV_PushMove_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x2A\x8B\xBC\x24\x88\x00\x00\x00\xD9"
#define SV_PushRotate_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x84\x24\x2A\x00\x00\x00\x2A\x8B\xBC\x24\xC0\x00\x00\x00\xD9"

#define CASHook_CASHook_Signature "\x8A\x44\x24\x2A\x2A\x54\x24\x2A\x2A\x8A\x5C\x24\x2A\x2A\x8B\xF1\xB9"
#define CASHook_Call_Signature "\x8B\x4C\x24\x04\x8D\x44\x24\x0C\x50\xFF\x74\x24\x0C\x6A\x00\xE8"
#define CASDocumentation_RegisterObjectType_Signature "\x68\x01\x00\x04\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8"
#define CASDocumentation_RegisterObjectProperty_Signature "\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8\x2A\x2A\x2A\x2A\x6A\x04"
#define CASDocumentation_RegisterObjectMethod_Signature "\x50\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x03"

#else

#define LOCATE_FROM_SIGNATURE(dll, sig) dlsym(dll, sig);

#define SV_PushEntity_Signature "_Z13SV_PushEntityP7edict_sPf"
#define SV_PushMove_Signature "_Z11SV_PushMoveP7edict_sf"
#define SV_PushRotate_Signature "_Z13SV_PushRotateP7edict_sf"

#define CASHook_CASHook_Signature "_ZN7CASHookC2EhhPKcS1_S1_RK16CASHookArguments"
#define CASHook_Call_Signature "_ZN7CASHook4CallEiz"
#define CASDocumentation_RegisterObjectType_Signature "_ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im"
#define CASDocumentation_RegisterObjectProperty_Signature "_ZN16CASDocumentation22RegisterObjectPropertyEPKcS1_S1_i"
#define CASDocumentation_RegisterObjectMethod_Signature "_ZN16CASDocumentation20RegisterObjectMethodEPKcS1_S1_RK10asSFuncPtrm"

#endif

#define FILL_FROM_SIGNATURE(dll, name) g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!g_pfn_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SIGNATURED_CALLER(dll, name, offset) auto Caller_of_##name = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!Caller_of_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate Caller of " #name " from " #dll " dll !");\
	return FALSE;\
}\
g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))GetCallAddress(Caller_of_##name + sizeof(name##_Signature) - 1 + (offset));\
if (!((char *)g_pfn_##name > (char*)dll && (char*)g_pfn_##name < (char*)dll + MH_GetModuleSize(dll)))\
{\
LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !"); \
return FALSE; \
}