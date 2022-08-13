#pragma once

#define IMPORT_FUNCTION_DEFINE(name) fn##name name;

#define IMPORT_FUNCTION_EXTERN(name) extern fn##name name;

#define PRIVATE_FUNCTION_DEFINE(name) fn##name g_pfn_##name; fn##name g_call_original_##name;

#define PRIVATE_FUNCTION_EXTERN(name) extern fn##name g_pfn_##name; extern fn##name g_call_original_##name;

#ifdef _WIN32

#define LOCATE_FROM_SIGNATURE(dll, sig) gpMetaUtilFuncs->pfnSearchPattern(dll##Base, gpMetaUtilFuncs->pfnGetImageSize(dll##Base), sig, sizeof(sig) - 1)

#define SV_PushEntity_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x8B\x84\x24\x2A\x00\x00\x00"
#define SV_PushMove_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x2A\x8B\xBC\x24\x88\x00\x00\x00\xD9"
#define SV_PushRotate_Signature "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x84\x24\x2A\x00\x00\x00\x2A\x8B\xBC\x24\xC0\x00\x00\x00\xD9"

#define sv_models_Signature "\x3D\xFE\x1F\x00\x00\x2A\x2A\x0F\xAE\xE8\xFF\x34\x8D"
#define host_frametime_Signature "\xD8\x83\xA8\x00\x00\x00\xD9\x9B\xA8\x00\x00\x00\xE8\x2A\x2A\x2A\x2A\xDD\x05"

#define ENGINE_DLL_NAME "hw.dll"

#else

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(A)   (sizeof(A)/sizeof((A)[0]))
#endif

#define LOCATE_FROM_SIGNATURE(dll, sig) DLSYM(dll##Handle, sig)

#define SV_PushEntity_Signature "_Z13SV_PushEntityP7edict_sPf"
#define SV_PushMove_Signature "_Z11SV_PushMoveP7edict_sf"
#define SV_PushRotate_Signature "_Z13SV_PushRotateP7edict_sf"

#define sv_Signature "sv"
#define sv_models_Signature "\x8B\x84\x82\x2A\x2A\x2A\x00\x89\x04\x24\xE8"
#define host_frametime_Signature "host_frametime"

#define ENGINE_DLL_NAME "hw.so"

#endif

#define IMPORT_FUNCTION_DLSYM(dll, name) name = (decltype(name))DLSYM((DLHANDLE)dll##Handle, #name);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to get " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SIGNATURE(dll, name) g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!g_pfn_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SIGNATURED_CALLER_FROM_START(dll, name, offset) auto Caller_of_##name = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!Caller_of_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate Caller of " #name " from " #dll " dll !");\
	return FALSE;\
}\
g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))gpMetaUtilFuncs->pfnGetNextCallAddr(Caller_of_##name + (offset), 1);\
if (!gpMetaUtilFuncs->pfnIsAddressInModuleRange((void *)g_pfn_##name, dll##Base))\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !"); \
	return FALSE; \
}

#define FILL_FROM_SIGNATURED_CALLER_FROM_END(dll, name, offset) auto Caller_of_##name = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!Caller_of_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate Caller of " #name " from " #dll " dll !");\
	return FALSE;\
}\
g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))gpMetaUtilFuncs->pfnGetNextCallAddr(Caller_of_##name + (sizeof(name##_Signature) - 1) + (offset), 1);\
if (!gpMetaUtilFuncs->pfnIsAddressInModuleRange((void *)g_pfn_##name, dll##Base))\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !"); \
	return FALSE; \
}

#define VAR_FROM_SIGNATURE(dll, name) name = (decltype(name))LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define VAR_FROM_SIGNATURE_FROM_START(dll, name, offset) auto name##_Temp = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!name##_Temp)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}\
name = *(decltype(name) *)(name##_Temp + offset);

#define VAR_FROM_SIGNATURE_FROM_END(dll, name, offset) auto name##_Temp = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature);\
if (!name##_Temp)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}\
name = *(decltype(name) *)(name##_Temp + (sizeof(name##_Signature) - 1) + offset);

#define INSTALL_INLINEHOOK(name) g_phook_##name = gpMetaUtilFuncs->pfnInlineHook((void*)g_pfn_##name, (void*)New##name, (void**)&g_call_original_##name, true)
#define UNINSTALL_HOOK(name) if(g_phook_##name) {gpMetaUtilFuncs->pfnUnHook(g_phook_##name); g_phook_##name = NULL;}