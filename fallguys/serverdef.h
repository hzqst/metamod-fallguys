#pragma once

#include "signatures.h"

//AngelScript const

const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

const int ASHookFlag_MapScript = 1;
const int ASHookFlag_Plugin = 2;

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

class CASDocumentation
{
public:

};

class CASMethodRegistration
{
public:
	CASMethodRegistration()
	{
		pfnMethod = NULL;
		unk1 = 0;
		unk2 = 0;
		unk3 = 0;
		unk4 = 0;
		unk5 = 0;
		unk6 = 1;
		unk7 = 0;
		unk8 = 3;
	}

	void *pfnMethod;//+0
	int unk1;//+4
	int unk2;//+8
	int unk3;//+12
	int unk4;//+16
	int unk5;//+20
	int unk6;//+24
	int unk7;//+28
	int unk8;//+32
};

#ifdef PLATFORM_WINDOWS

#define SC_SERVER_DECL __fastcall
#define SC_SERVER_DUMMYARG_NOCOMMA , int dummy
#define SC_SERVER_DUMMYARG int dummy, 
#define SC_SERVER_PASS_DUMMYARG dummy, 
#define SERVER_DLL_NAME "server.dll"

#else

#define SC_SERVER_DECL 
#define SC_SERVER_DUMMYARG_NOCOMMA
#define SC_SERVER_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG
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
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectType)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int a4, unsigned int flags);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectType);
int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* docs, const char* name, int a4, unsigned int flags);

//CASDocumentation::RegisterObjectProperty __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectProperty)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, const char *prop, int offset);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectProperty);

//CASDocumentation::RegisterObjectMethod __fastcall in Windows
typedef int (SC_SERVER_DECL *fnCASDocumentation_RegisterObjectMethod)(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *objectname, const char *funcname, CASMethodRegistration *reg, int a5);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_RegisterObjectMethod);