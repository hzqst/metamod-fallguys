#pragma once

#define PRIVATE_FUNCTION_DEFINE(name) fn##name g_pfn_##name; fn##name g_call_original_##name;

#define PRIVATE_FUNCTION_EXTERN(name) extern fn##name g_pfn_##name; extern fn##name g_call_original_##name;

#ifdef _WIN32

#define LOCATE_FROM_SIGNATURE(dll, sig) gpMetaUtilFuncs->pfnSearchPattern(dll##Base, gpMetaUtilFuncs->pfnGetModuleSize(dll##Base), sig, sizeof(sig) - 1)

#define CASHook_CASHook_Signature "\x8A\x44\x24\x2A\x2A\x54\x24\x2A\x2A\x8A\x5C\x24\x2A\x2A\x8B\xF1\xB9"
#define CASHook_Call_Signature "\x8B\x4C\x24\x04\x8D\x44\x24\x0C\x50\xFF\x74\x24\x0C\x6A\x00\xE8"
#define CASDocumentation_RegisterObjectType_Signature "\x68\x01\x00\x04\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8"
#define CASDocumentation_RegisterObjectProperty_Signature "\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8\x2A\x2A\x2A\x2A\x6A\x04"
#define CASDocumentation_RegisterObjectMethod_Signature "\x50\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x03"

#else

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(A)   (sizeof(A)/sizeof((A)[0]))
#endif

#define LOCATE_FROM_SIGNATURE(dll, sig) DLSYM(dll##Handle, sig)

#define CASHook_CASHook_Signature "_ZN7CASHookC2EhhPKcS1_S1_RK16CASHookArguments"
#define CASHook_Call_Signature "_ZN7CASHook4CallEiz"
#define CASDocumentation_RegisterObjectType_Signature "_ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im"
#define CASDocumentation_RegisterObjectProperty_Signature "_ZN16CASDocumentation22RegisterObjectPropertyEPKcS1_S1_i"
#define CASDocumentation_RegisterObjectMethod_Signature "_ZN16CASDocumentation20RegisterObjectMethodEPKcS1_S1_RK10asSFuncPtrm"

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
if (!gpMetaUtilFuncs->pfnIsAddressInModuleRange(((void *)g_pfn_##name, dll##Base))\
{\
LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll ! got %p", ((void *)g_pfn_##name); \
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

#define INSTALL_INLINEHOOK(name) gpMetaUtilFuncs->pfnInlineHook((void*)g_pfn_##name, (void*)New##name, (void**)&g_call_original_##name, true);