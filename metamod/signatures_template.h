#ifndef SIGNATURE_TEMPLATE_H
#define SIGNATURE_TEMPLATE_H

#define IMPORT_FUNCTION_POINTER_DEFINE(name) fn##name *name = NULL;

#define IMPORT_FUNCTION_DEFINE(name) fn##name name;

#define IMPORT_FUNCTION_EXTERN(name) extern fn##name name;

#define PRIVATE_FUNCTION_DEFINE(name) fn##name g_pfn_##name; fn##name g_call_original_##name;

#define PRIVATE_FUNCTION_EXTERN(name) extern fn##name g_pfn_##name; extern fn##name g_call_original_##name;

#define LOCATE_FROM_SIGNATURE(dll, sig) gpMetaUtilFuncs->pfnSearchPattern(dll##Base, dll##Size, sig, sizeof(sig) - 1)
#define LOCATE_FROM_SIGNATURE_FROM_FUNCTION(func, size, sig) gpMetaUtilFuncs->pfnSearchPattern(func, size, sig, sizeof(sig) - 1)
#define LOCATE_FROM_SYMBOL(dll, sym) gpMetaUtilFuncs->pfnGetProcAddress(dll##Handle, sym)

#ifdef _WIN32

#define ENGINE_DLL_NAME "hw.dll"

#else

#ifndef _ARRAYSIZE
#define _ARRAYSIZE(A)   (sizeof(A)/sizeof((A)[0]))
#endif

#define ENGINE_DLL_NAME "hw.so"

#endif

#define IMPORT_FUNCTION_DLSYM(dll, name) name = (decltype(name))DLSYM((DLHANDLE)dll##Handle, #name);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to get " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SIGNATURE_NO_CHECK(dll, name) g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))LOCATE_FROM_SIGNATURE(dll, name##_Signature)

#define FILL_FROM_SIGNATURE(dll, name) FILL_FROM_SIGNATURE_NO_CHECK(dll, name);\
if (!g_pfn_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SIGNATURE_TY(dll, name, ty) g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))LOCATE_FROM_SIGNATURE(dll, name##_Signature_##ty);\
if (!g_pfn_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define FILL_FROM_SYMBOL_NO_CHECK(dll, name) g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))LOCATE_FROM_SYMBOL(dll, name##_Symbol)

#define FILL_FROM_SYMBOL(dll, name) FILL_FROM_SYMBOL_NO_CHECK(dll, name);\
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

#define FILL_FROM_SIGNATURED_TY_CALLER_FROM_START(dll, name, ty, offset) auto Caller_of_##name = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature_##ty);\
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

#define FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(dll, name, ty, offset) auto Caller_of_##name = (char *)LOCATE_FROM_SIGNATURE(dll, name##_Signature_##ty);\
if (!Caller_of_##name)\
{\
	LOG_ERROR(PLID, "Failed to locate Caller of " #name " from " #dll " dll !");\
	return FALSE;\
}\
g_pfn_##name = g_call_original_##name = (decltype(g_pfn_##name))gpMetaUtilFuncs->pfnGetNextCallAddr(Caller_of_##name + (sizeof(name##_Signature_##ty) - 1) + (offset), 1);\
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

#define VAR_FROM_SYMBOL(dll, name) name = (decltype(name))LOCATE_FROM_SYMBOL(dll, name##_Symbol);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}

#define VAR_FROM_SIGNATURE_RENAME(dll, name, realname) name = (decltype(name))LOCATE_FROM_SIGNATURE(dll, realname##_Signature);\
if (!name)\
{\
	LOG_ERROR(PLID, "Failed to locate " #realname " from " #dll " dll !");\
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

#define VAR_FROM_SIGNATURE_FROM_FUNCTION_FROM_START(dll, func, size, name, offset) auto name##_Temp = (char *)LOCATE_FROM_SIGNATURE_FROM_FUNCTION(func, size, name##_Signature);\
if (!name##_Temp)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}\
name = *(decltype(name) *)(name##_Temp + offset);

#define VAR_FROM_SIGNATURE_FROM_FUNCTION_FROM_END(dll, func, size, name, offset) auto name##_Temp = (char *)LOCATE_FROM_SIGNATURE_FROM_FUNCTION(func, size, name##_Signature);\
if (!name##_Temp)\
{\
	LOG_ERROR(PLID, "Failed to locate " #name " from " #dll " dll !");\
	return FALSE;\
}\
name = *(decltype(name) *)(name##_Temp + (sizeof(name##_Signature) - 1) + offset);

#define INSTALL_INLINEHOOK(name) g_phook_##name = gpMetaUtilFuncs->pfnInlineHook((void*)g_pfn_##name, (void*)New##name, (void**)&g_call_original_##name, true)
#define UNINSTALL_HOOK(name) if(g_phook_##name) {gpMetaUtilFuncs->pfnUnHook(g_phook_##name); g_phook_##name = NULL;}

#endif /* META_API_H */
