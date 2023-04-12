#pragma once

const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

const int ASFlag_MapScript = 1;
const int ASFlag_Plugin = 2;

const int ASFileAccessControl_Read = 1;
const int ASFileAccessControl_Write = 2;

enum asECallConvTypes
{
	asCALL_CDECL = 0,
	asCALL_STDCALL = 1,
	asCALL_THISCALL_ASGLOBAL = 2,
	asCALL_THISCALL = 3,
	asCALL_CDECL_OBJLAST = 4,
	asCALL_CDECL_OBJFIRST = 5,
	asCALL_GENERIC = 6,
	asCALL_THISCALL_OBJLAST = 7,
	asCALL_THISCALL_OBJFIRST = 8
};

// Object type flags
enum asEObjTypeFlags
{
	asOBJ_REF = (1 << 0),
	asOBJ_VALUE = (1 << 1),
	asOBJ_GC = (1 << 2),
	asOBJ_POD = (1 << 3),
	asOBJ_NOHANDLE = (1 << 4),
	asOBJ_SCOPED = (1 << 5),
	asOBJ_TEMPLATE = (1 << 6),
	asOBJ_ASHANDLE = (1 << 7),
	asOBJ_APP_CLASS = (1 << 8),
	asOBJ_APP_CLASS_CONSTRUCTOR = (1 << 9),
	asOBJ_APP_CLASS_DESTRUCTOR = (1 << 10),
	asOBJ_APP_CLASS_ASSIGNMENT = (1 << 11),
	asOBJ_APP_CLASS_COPY_CONSTRUCTOR = (1 << 12),
	asOBJ_APP_CLASS_C = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR),
	asOBJ_APP_CLASS_CD = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR),
	asOBJ_APP_CLASS_CA = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_CK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_CDA = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_CDK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_CAK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_CDAK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_CONSTRUCTOR + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_D = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR),
	asOBJ_APP_CLASS_DA = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_DK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_DAK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_DESTRUCTOR + asOBJ_APP_CLASS_ASSIGNMENT + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_A = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_ASSIGNMENT),
	asOBJ_APP_CLASS_AK = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_ASSIGNMENT + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_K = (asOBJ_APP_CLASS + asOBJ_APP_CLASS_COPY_CONSTRUCTOR),
	asOBJ_APP_CLASS_MORE_CONSTRUCTORS = (1 << 31),
	asOBJ_APP_PRIMITIVE = (1 << 13),
	asOBJ_APP_FLOAT = (1 << 14),
	asOBJ_APP_ARRAY = (1 << 15),
	asOBJ_APP_CLASS_ALLINTS = (1 << 16),
	asOBJ_APP_CLASS_ALLFLOATS = (1 << 17),
	asOBJ_NOCOUNT = (1 << 18),
	asOBJ_APP_CLASS_ALIGN8 = (1 << 19),
	asOBJ_IMPLICIT_HANDLE = (1 << 20),
	asOBJ_MASK_VALID_FLAGS = 0x801FFFFF,
	// Internal flags
	asOBJ_SCRIPT_OBJECT = (1 << 21),
	asOBJ_SHARED = (1 << 22),
	asOBJ_NOINHERIT = (1 << 23),
	asOBJ_FUNCDEF = (1 << 24),
	asOBJ_LIST_PATTERN = (1 << 25),
	asOBJ_ENUM = (1 << 26),
	asOBJ_TEMPLATE_SUBTYPE = (1 << 27),
	asOBJ_TYPEDEF = (1 << 28),
	asOBJ_ABSTRACT = (1 << 29),
	asOBJ_APP_ALIGN16 = (1 << 30)
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

class asContext
{
public:
	virtual void unk0() = 0;
	virtual void unk1() = 0;
	virtual void unk2() = 0;
	virtual void unk3() = 0;
	virtual void unk4() = 0;
	virtual void unk5() = 0;
	virtual void unk6() = 0;
	virtual void unk7() = 0;
	virtual void unk8() = 0;
	virtual void unk9() = 0;
	virtual void unk10() = 0;
	virtual void unk11() = 0;
	virtual void unk12() = 0;
	virtual void unk13() = 0;
	virtual void unk14() = 0;
	virtual void unk15() = 0;
	virtual void unk16() = 0;
	virtual void unk17() = 0;
	virtual void unk18() = 0;
	virtual void unk19() = 0;
	virtual void unk20() = 0;
	virtual void unk21() = 0;
	virtual void unk22() = 0;
	virtual void unk23() = 0;
	virtual int unk24() = 0;
	virtual int unk25() = 0;
	virtual int unk26() = 0;
};

class aslScriptFunction
{
public:
	
};

class CASFunction
{
public:
#ifdef _WIN32
#define CASFunction_dtor_firstarg 1
	virtual void Release(char a1) = 0;
#else
#define CASFunction_dtor_firstarg
	virtual void Unknown() = 0;
	virtual void Release() = 0;
#endif

	void *getReference()
	{
		return &ref;
	}

	aslScriptFunction *getScriptFunction()
	{
		return aslfn;
	}

	CASModule *getModule()
	{
		return asmodule;
	}

	asContext *getContext()
	{
		return ascontext;
	}

	int ref;
	aslScriptFunction *aslfn;
	CASModule *asmodule;
	asContext *ascontext;
};

#define ASEXT_DereferenceCASFunction(callback)	if (ASEXT_CASRefCountedBaseClass_InternalRelease(callback->getReference()))\
{\
	callback->Release(CASFunction_dtor_firstarg);\
}

class asIScriptEngine;

class CASServerManager
{
public:
	int unk1;//0
	int unk2;//4
	int unk3;//8
	asIScriptEngine *scriptEngine;//12
	int unk5;//16
	int unk6;//20
	CASModule *curModule;//24
};

typedef void(*fnASDocInitCallback)(CASDocumentation *pASDoc);
typedef void(*fnASDirInitCallback)(CASDirectoryList *pASDir);

/*
	Callbacks must be registered before AngelScript initialization
*/
typedef bool(*fnASEXT_RegisterDocInitCallback)(fnASDocInitCallback callback);

extern fnASEXT_RegisterDocInitCallback ASEXT_RegisterDocInitCallback;


/*
	Callbacks must be registered before AngelScript initialization
*/

typedef bool(*fnASEXT_RegisterDirInitCallback)(fnASDirInitCallback callback);

extern fnASEXT_RegisterDirInitCallback ASEXT_RegisterDirInitCallback;

/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterObjectMethod)(CASDocumentation *pASDoc, const char *docs, const char *name, const char *func, void *pfn, int type);

extern fnASEXT_RegisterObjectMethod ASEXT_RegisterObjectMethod;

const int ObjectBehaviour_Constructor = 0;
const int ObjectBehaviour_Destructor = 2;

/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterObjectBehaviour)(CASDocumentation *pASDoc, const char *docs, const char *name, int behaviour, const char *func, void *pfn, int type);

extern fnASEXT_RegisterObjectBehaviour ASEXT_RegisterObjectBehaviour;

/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterObjectType)(CASDocumentation *pASDoc, const char *docs, const char *name, int unk, unsigned int flags);

extern fnASEXT_RegisterObjectType ASEXT_RegisterObjectType;

/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterObjectProperty)(CASDocumentation *pASDoc, const char *docs, const char *name, const char *prop, int offset);

extern fnASEXT_RegisterObjectProperty ASEXT_RegisterObjectProperty;


/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterGlobalProperty)(CASDocumentation *pASDoc, const char *docs, const char *name, void *ptr);

extern fnASEXT_RegisterGlobalProperty ASEXT_RegisterGlobalProperty;

/*
	Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_RegisterFuncDef)(CASDocumentation *pASDoc, const char *docs, const char *funcdef);

extern fnASEXT_RegisterFuncDef ASEXT_RegisterFuncDef;

typedef void(*fnASEXT_CreateDirectory)(void *pASDir, const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk);
extern fnASEXT_CreateDirectory ASEXT_CreateDirectory;

typedef void(*fnASEXT_CStringAssign)(void *pthis, const char *src, size_t len);

extern fnASEXT_CStringAssign ASEXT_CStringAssign;

typedef void(*fnASEXT_CStringdtor)(void *pthis);

extern fnASEXT_CStringdtor ASEXT_CStringdtor;

/*
	Must be registered before call hook
*/
typedef void *(*fnASEXT_RegisterHook)(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args);

extern fnASEXT_RegisterHook ASEXT_RegisterHook;

typedef void(*fnASEXT_CallHook)(void *hook, int unk, ...);

extern fnASEXT_CallHook *ASEXT_CallHook;

typedef int(*fnASEXT_CallCASBaseCallable)(void *callable, int dummy, ...);

extern fnASEXT_CallCASBaseCallable *ASEXT_CallCASBaseCallable;

typedef CASServerManager *(*fnASEXT_GetServerManager)();

extern fnASEXT_GetServerManager ASEXT_GetServerManager;

typedef CASFunction *(*fnASEXT_CreateCASFunction)(aslScriptFunction *aslfn, CASModule *asmodule, int unk);

extern fnASEXT_CreateCASFunction ASEXT_CreateCASFunction;

typedef bool(*fnASEXT_CASRefCountedBaseClass_InternalRelease)(void *ref);

extern fnASEXT_CASRefCountedBaseClass_InternalRelease ASEXT_CASRefCountedBaseClass_InternalRelease;

typedef void(*fnASEXT_CScriptAny_Release)(void *anywhat);

extern fnASEXT_CScriptAny_Release ASEXT_CScriptAny_Release;

#ifdef _WIN32

#define SC_SERVER_DECL __fastcall
#define SC_SERVER_DUMMYARG_NOCOMMA , int dummy
#define SC_SERVER_DUMMYARG int dummy, 
#define SC_SERVER_PASS_DUMMYARG dummy, 
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA dummy

#else

#define SC_SERVER_DECL 
#define SC_SERVER_DUMMYARG_NOCOMMA
#define SC_SERVER_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA

#endif

class CString
{
public:
	const char *c_str() const
	{
		return m_ptr;
	}
	size_t length() const
	{
		return m_len;
	}
	char chatAt(size_t offset) const
	{
		return offset >= m_len ? m_ptr[0] : m_ptr[offset];
	}
	bool empty() const
	{
		return m_len == 0;
	}
	void assign(const char *src, size_t len)
	{
		ASEXT_CStringAssign(this, src, len);
	}
	void dtor()
	{
		ASEXT_CStringdtor(this);
	}

	int unk1;//0
	int unk2;//4
	int unk3;//8
	int unk4;//12
	int unk5;//16
	char * m_ptr;//20
	size_t m_len;//24
	size_t m_capacity;//28
	int unk7;//32
};

class CASBLOB
{
public:
	void *data() const
	{
		return m_buf;
	}
	size_t size() const
	{
		return m_size;
	}

	bool empty() const
	{
		return m_size == 0;
	}

	int unk1;//0
	void *m_buf;//4
	size_t m_size;//8
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

#define IMPORT_ASEXT_API(asext) IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallHook);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallCASBaseCallable);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterDocInitCallback);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterDirInitCallback);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectMethod);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectBehaviour);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectType);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterObjectProperty);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterHook);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterFuncDef);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateDirectory);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CStringAssign);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CStringdtor);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_GetServerManager);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateCASFunction);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CASRefCountedBaseClass_InternalRelease);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CScriptAny_Release);


#define IMPORT_ASEXT_API_DEFINE() IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDocInitCallback);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDirInitCallback);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectMethod);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectBehaviour);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectType);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectProperty);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterFuncDef);\
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterHook);\
IMPORT_FUNCTION_DEFINE(ASEXT_CreateDirectory);\
IMPORT_FUNCTION_DEFINE(ASEXT_CStringAssign);\
IMPORT_FUNCTION_DEFINE(ASEXT_CStringdtor);\
IMPORT_FUNCTION_DEFINE(ASEXT_GetServerManager);\
IMPORT_FUNCTION_DEFINE(ASEXT_CreateCASFunction);\
IMPORT_FUNCTION_DEFINE(ASEXT_CASRefCountedBaseClass_InternalRelease);\
IMPORT_FUNCTION_DEFINE(ASEXT_CScriptAny_Release);\
fnASEXT_CallHook *ASEXT_CallHook = NULL;\
fnASEXT_CallCASBaseCallable *ASEXT_CallCASBaseCallable = NULL;

#define REGISTER_PLAIN_VALUE_OBJECT(name) ASEXT_RegisterObjectType(pASDoc, #name" plain value object", #name, sizeof(name), asOBJ_VALUE);\
ASEXT_RegisterObjectBehaviour(pASDoc, "Default constructor",  #name, ObjectBehaviour_Constructor, "void "#name"()", (void *)name##_ctor, 4);\
ASEXT_RegisterObjectBehaviour(pASDoc, "Copy constructor",  #name, ObjectBehaviour_Constructor, "void "#name"(const "#name"& in other)", (void *)name##_copyctor, 4);\
ASEXT_RegisterObjectBehaviour(pASDoc, "Destructor",  #name, ObjectBehaviour_Destructor, "void Destruct"#name"()", (void *)name##_dtor, 4);\
ASEXT_RegisterObjectMethod(pASDoc, "operator=",  #name,  #name"& opAssign(const "#name"& in other)", (void *)name##_opassign, 3);

#define EXTERN_PLAIN_VALUE_OBJECT(name) void name##_ctor(name *pthis);\
void name##_copyctor(name *a1, name *a2);\
void name##_dtor(name *pthis);\
name * SC_SERVER_DECL name##_opassign(name *a1, SC_SERVER_DUMMYARG name *a2);
