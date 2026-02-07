#pragma once

#include "signatures.h"

//AngelScript const

class CASHookRegistration
{
public:
	int unk;
	int stopMode;
	const char *docs;
};

class CASHook
{
public:
	CASHook *pnext;//0
	unsigned char flags;//4
	unsigned char type;//5
	unsigned char padding;//6
	unsigned char padding2;//7
	const char *firstClass;//8
	const char *funcName;//12
	const char *argList;//16
	const char *docs;//20
	int unk1;//24
	int unk2;//28
	int unk3;//32
	int unk4;//36
};

class CString
{
public:
	const char *c_str()
	{
		return m_ptr;
	}
	char charAt(size_t offset)
	{
		return offset >= m_len ? m_ptr[0] : m_ptr[offset];
	}
	bool empty()
	{
		return m_len == 0;
	}

	int unk1;//0
	int unk2;//4
	int unk3;//8
	int unk4;//12
	int unk5;//16
	char * m_ptr;//20
	size_t m_len;//24
	int unk6;//28
	int unk7;//32
};

class CASBLOB
{
public:
	void *data()
	{
		return m_buf;
	}
	bool empty()
	{
		return m_size == 0;
	}

	int unk1;//0
	void *m_buf;//4
	int m_size;//8
	int m_read_offset;//12
	int m_write_offset;//16
	char m_can_resize;//20
};

class CScriptArrayBuffer
{
public:
	void *data()
	{
		return m_buf;
	}
	size_t size()
	{
		return m_size;
	}
	int m_unk1;//0
	int m_size;//4
	char m_buf[1];//+8
};

class CScriptArray
{
public:
	void *data()
	{
		return m_buffer->data();
	}

	size_t size()
	{
		return m_buffer->m_size;
	}

	int m_unk1;//0
	int m_unk2;//4
	int m_unk3;//8
	int m_unk4;//12
	CScriptArrayBuffer *m_buffer;//16
	int m_unk5;//20
	int m_ElementTypeId;//24
};

class CASDocumentation
{
public:

};

class CASDirectoryList
{
public:

};

class CASModule
{
public:

};

class aslScriptFunction
{
public:

};

class CScriptDictionary
{
public:

};

class CASFunction
{
public:
#ifdef _WIN32
	virtual void Unknown() = 0;
#endif
	virtual void Release(void *pthis) = 0;

	void *getReference()
	{
		return &ref;
	}

	int ref;
};

class CASServerManager
{
public:
	int unk1;//0
	int unk2;//4
	int unk3;//8
	void *scriptManager;//12
	int unk5;//16
	int unk6;//20
	CASModule *curModule;//24
};

class asSFuncPtr
{
public:
	asSFuncPtr()
	{
		pfn = NULL;
		unk1 = 0;
		unk2 = 0;
		unk3 = 0;
		unk4 = 0;
		unk5 = 0;
		unk6 = 1;
		unk7 = 0;
		flag = 3;
	}

	void *pfn;//+0
	int unk1;//+4
	int unk2;//+8
	int unk3;//+C
	int unk4;//+0x10
	int unk5;//+0x14
	int unk6;//+0x18
	int unk7;//+0x1C
	unsigned char flag;//+0x20  // 1 = generic, 2 = global func, 3 = method
};

class CScriptBuilder;

#ifdef _WIN32

#define SC_SERVER_DECL __fastcall
#define SC_SERVER_DUMMYARG_NOCOMMA , int dummy
#define SC_SERVER_DUMMYARG int dummy, 
#define SC_SERVER_PASS_DUMMYARG dummy, 
#define SC_SERVER_PASS_DUMMYARG2 ,dummy
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA dummy
#define SC_SERVER_DUMMYVAR int dummy = 0;
#define SERVER_DLL_NAME "server.dll"

#else

#define SC_SERVER_DECL 
#define SC_SERVER_DUMMYARG_NOCOMMA
#define SC_SERVER_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG2 
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA
#define SC_SERVER_DUMMYVAR
#define SERVER_DLL_NAME "server.so"

#endif

//Server dll private functions

//CASHook::CASHook __fastcall in Windows
typedef CASHook* (SC_SERVER_DECL* fnCASHook_CASHook)(CASHook* pthis, SC_SERVER_DUMMYARG unsigned char flags, unsigned char type, const char* firstClass, const char* funcName, const char* argList, CASHookRegistration* reg);
PRIVATE_FUNCTION_EXTERN(CASHook_CASHook);

//CASHook::Call __cdecl in Windows
typedef void(*fnCASHook_Call)(CASHook *pthis, int unk, ...); 
PRIVATE_FUNCTION_EXTERN(CASHook_Call);

//CASDocumentation::RegisterObjectType __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectType)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int size, unsigned int flags);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectType);
int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* docs, const char* name, int size, unsigned int flags);

//CASDocumentation::RegisterObjectProperty __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectProperty)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, const char *prop, int offset);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectProperty);

//CASDocumentation::RegisterGlobalProperty __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterGlobalProperty)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, void *ptr);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterGlobalProperty);

//CASDocumentation::RegisterObjectMethod __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectMethod)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *objectname, const char *func, asSFuncPtr *reg, int a5);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectMethod);

//CASDocumentation::RegisterObjectBehaviour __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectBehaviour)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *objectname, int behaviour, const char *func, asSFuncPtr *reg, int a6, int a7);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectBehaviour);

typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterFuncDef)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *funcdef);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterFuncDef);

typedef void (SC_SERVER_DECL* fnCASDocumentation_RegisterEnum)(CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* docs, const char* enums, int enumtype);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterEnum);

typedef void (SC_SERVER_DECL* fnCASDocumentation_RegisterEnumValue)(CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* docs, const char* enums, const char* name, int value);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterEnumValue);

typedef void (SC_SERVER_DECL* fnCASDocumentation_SetDefaultNamespace)(CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* ns);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_SetDefaultNamespace);

typedef void (SC_SERVER_DECL *fnCASDirectoryList_CreateDirectory)(CASDirectoryList *pthis, SC_SERVER_DUMMYARG const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk);
PRIVATE_FUNCTION_EXTERN(CASDirectoryList_CreateDirectory);
void SC_SERVER_DECL NewCASDirectoryList_CreateDirectory(CASDirectoryList* pthis, SC_SERVER_DUMMYARG const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk);

typedef CASFunction *(*fnCASFunction_Create)(aslScriptFunction *aslfn, CASModule *asmodule, bool unk);
PRIVATE_FUNCTION_EXTERN(CASFunction_Create);

typedef bool (SC_SERVER_DECL *fnCASRefCountedBaseClass_InternalRelease)(void *ref);
PRIVATE_FUNCTION_EXTERN(CASRefCountedBaseClass_InternalRelease);

typedef void (SC_SERVER_DECL *fnCScriptAny_Release)(void *anywhat);
PRIVATE_FUNCTION_EXTERN(CScriptAny_Release);

typedef void (SC_SERVER_DECL* fnCScriptArray_Release)(void* anywhat);
PRIVATE_FUNCTION_EXTERN(CScriptArray_Release);

typedef void (*fnCASBaseCallable_Call)(CASFunction *, int dummy, ...);
PRIVATE_FUNCTION_EXTERN(CASBaseCallable_Call);

typedef void (SC_SERVER_DECL *fnCString_Assign)(CString *pthis, SC_SERVER_DUMMYARG const char *src, size_t len);
PRIVATE_FUNCTION_EXTERN(CString_Assign);

typedef void (SC_SERVER_DECL *fnCString_dtor)(CString *pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CString_dtor);

typedef bool (SC_SERVER_DECL *fnCASBLOB_ReadData)(CASBLOB *pthis, SC_SERVER_DUMMYARG void *outbuf, size_t read_bytes);
PRIVATE_FUNCTION_EXTERN(CASBLOB_ReadData);

typedef bool (SC_SERVER_DECL *fnCASBLOB_WriteData)(CASBLOB *pthis, SC_SERVER_DUMMYARG  void *outbuf, size_t read_bytes);
PRIVATE_FUNCTION_EXTERN(CASBLOB_WriteData);

typedef void* (*fnasGetActiveContext)();
PRIVATE_FUNCTION_EXTERN(asGetActiveContext);

//https://www.angelcode.com/angelscript/sdk/docs/manual/doc_addon_build.html
typedef void (SC_SERVER_DECL *fnCScriptBuilder_DefineWord)(CScriptBuilder* pthis, SC_SERVER_DUMMYARG const char* word);
PRIVATE_FUNCTION_EXTERN(CScriptBuilder_DefineWord);
void SC_SERVER_DECL NewCScriptBuilder_DefineWord(CScriptBuilder* pthis, SC_SERVER_DUMMYARG const char* word);

// CScriptDictionary functions
typedef CScriptDictionary* (*fnCScriptDictionary_Create)(void* pScriptEngine);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Create);

typedef void (SC_SERVER_DECL* fnCScriptDictionary_AddRef)(CScriptDictionary* pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_AddRef);

typedef void (SC_SERVER_DECL *fnCScriptDictionary_Release)(CScriptDictionary* pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Release);

typedef void (SC_SERVER_DECL *fnCScriptDictionary_Set)(CScriptDictionary* pthis, SC_SERVER_DUMMYARG const CString* key, const void* val, int asTypeId);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Set);

typedef bool (SC_SERVER_DECL *fnCScriptDictionary_Get)(CScriptDictionary* pthis, SC_SERVER_DUMMYARG const CString* key, void* outval, int asTypeId);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Get);

typedef bool (SC_SERVER_DECL *fnCScriptDictionary_Exists)(CScriptDictionary* pthis, SC_SERVER_DUMMYARG const CString* key);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Exists);

typedef bool (SC_SERVER_DECL *fnCScriptDictionary_IsEmpty)(CScriptDictionary* pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_IsEmpty);

typedef unsigned int (SC_SERVER_DECL *fnCScriptDictionary_GetSize)(CScriptDictionary* pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_GetSize);

typedef void (SC_SERVER_DECL *fnCScriptDictionary_Delete)(CScriptDictionary* pthis, SC_SERVER_DUMMYARG const CString* key);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_Delete);

typedef void (SC_SERVER_DECL *fnCScriptDictionary_DeleteAll)(CScriptDictionary* pthis SC_SERVER_DUMMYARG_NOCOMMA);
PRIVATE_FUNCTION_EXTERN(CScriptDictionary_DeleteAll);

extern CASServerManager **g_pServerManager;