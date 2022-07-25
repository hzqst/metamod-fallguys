#pragma once


const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

const int ASFlag_MapScript = 1;
const int ASFlag_Plugin = 2;

const int ASFileAccessControl_Read = 1;
const int ASFileAccessControl_Write = 2;

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

class CASFunction
{
public:
	virtual void Release(int what) = 0;

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
	int unk4;//12
	int unk5;//16
	int unk6;//20
	CASModule *curModule;//24
};

typedef void(*fnASDocInitCallback)(CASDocumentation *pASDoc);
typedef void(*fnASDirInitCallback)(CASDirectoryList *pASDir);

/*
	Callbacks must be registered before AngelScript initialization
*/
typedef bool (*fnASEXT_RegisterDocInitCallback)(fnASDocInitCallback callback);

extern fnASEXT_RegisterDocInitCallback ASEXT_RegisterDocInitCallback;


/*
	Callbacks must be registered before AngelScript initialization
*/

typedef bool(*fnASEXT_RegisterDirInitCallback)(fnASDirInitCallback callback);

extern fnASEXT_RegisterDirInitCallback ASEXT_RegisterDirInitCallback;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectMethod)(CASDocumentation *pASDoc, const char *docs, const char *name, const char *func, void *pfn, int type);

extern fnASEXT_RegisterObjectMethod ASEXT_RegisterObjectMethod;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectType)(CASDocumentation *pASDoc, const char *docs, const char *name, int unk, unsigned int flags);

extern fnASEXT_RegisterObjectType ASEXT_RegisterObjectType;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectProperty)(CASDocumentation *pASDoc, const char *docs, const char *name, const char *prop, int offset);

extern fnASEXT_RegisterObjectProperty ASEXT_RegisterObjectProperty;
/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterFuncDef)(CASDocumentation *pASDoc, const char *docs, const char *funcdef);

extern fnASEXT_RegisterFuncDef ASEXT_RegisterFuncDef;

typedef void (*fnASEXT_CreateDirectory)(void *pASDir, const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk);
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

typedef void(*fnASEXT_CallCASBaseCallable)(void *callable, int dummy, ...);

extern fnASEXT_CallCASBaseCallable *ASEXT_CallCASBaseCallable;

typedef CASServerManager *(*fnASEXT_GetServerManager)();

extern fnASEXT_GetServerManager ASEXT_GetServerManager;

typedef CASFunction *(*fnASEXT_CreateCASFunction)(aslScriptFunction *aslfn, CASModule *asmodule, int unk);

extern fnASEXT_CreateCASFunction ASEXT_CreateCASFunction;

typedef bool (*fnASEXT_CASRefCountedBaseClass_InternalRelease)(void *ref);

extern fnASEXT_CASRefCountedBaseClass_InternalRelease ASEXT_CASRefCountedBaseClass_InternalRelease;

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