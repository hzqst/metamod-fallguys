#pragma once

#define IMPORT_FUNCTION_DEFINE(name) fn##name name;

#define IMPORT_FUNCTION_EXTERN(name) extern fn##name name;

#define PRIVATE_FUNCTION_DEFINE(name) fn##name g_pfn_##name; fn##name g_call_original_##name;

#define PRIVATE_FUNCTION_EXTERN(name) extern fn##name g_pfn_##name; extern fn##name g_call_original_##name;

#ifdef _WIN32

#define LOCATE_FROM_SIGNATURE(dll, sig) gpMetaUtilFuncs->pfnSearchPattern(dll##Base, gpMetaUtilFuncs->pfnGetModuleSize(dll##Base), sig, sizeof(sig) - 1)

#define curl_easy_init_Signature "curl_easy_init"

#define LIBCURL_DLL_NAME "libcurl.dll"

#else

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(A)   (sizeof(A)/sizeof((A)[0]))
#endif

#define LOCATE_FROM_SIGNATURE(dll, sig) DLSYM(dll##Handle, sig)

#define curl_easy_init_Signature "curl_easy_init"

#define LIBCURL_DLL_NAME "libcurl.so.4"

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