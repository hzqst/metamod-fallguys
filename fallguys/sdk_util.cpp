// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// Selected portions of dlls/util.cpp from SDK 2.1.
// Functions copied from there as needed...
// And modified to avoid buffer overflows (argh).

/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include <extdll.h>
#include <enginecallback.h>		// ALERT()

#include "osdep.h"				// win32 vsnprintf, etc

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( const char *fmt, ... )
{
	va_list			argptr;
	static char		string[1024];
	
	va_start ( argptr, fmt );
	_vsnprintf ( string, sizeof(string), fmt, argptr );
	va_end   ( argptr );

	// Print to server console
	ALERT( at_logged, "%s", string );
}

void *MH_SearchPattern(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen)
{
	char * dwStartAddr = (char *)pStartSearch;
	char * dwEndAddr = dwStartAddr + dwSearchLen - dwPatternLen;

	while (dwStartAddr < dwEndAddr)
	{
		bool found = true;

		for (size_t i = 0; i < dwPatternLen; i++)
		{
			char code = *(char *)(dwStartAddr + i);

			if (pPattern[i] != 0x2A && pPattern[i] != code)
			{
				found = false;
				break;
			}
		}

		if (found)
			return (void *)dwStartAddr;

		dwStartAddr++;
	}

	return NULL;
}

size_t MH_GetModuleSize(void *hModule)
{
#ifdef PLATFORM_WINDOWS
	return ((IMAGE_NT_HEADERS *)((char *)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
#else
	return 0;//wtf?
#endif
}

void *MH_GetModuleBase(const char *name)
{
#ifdef PLATFORM_WINDOWS
	return (void *)GetModuleHandleA(name);
#else
	return (void *)dlopen(name, RTLD_NOLOAD);
#endif
}

bool MH_IsAddressInModule(void *lpAddress, void *hModule)
{
#ifdef PLATFORM_WINDOWS
	return (char *)lpAddress > (char *)hModule && (char *)lpAddress < (char *)hModule + MH_GetModuleSize(hModule);
#else
	Dl_info info;
	if (dladdr(lpAddress, &info) != 0 && info.dli_fbase == hModule)
		return true;

	return false;
#endif
}