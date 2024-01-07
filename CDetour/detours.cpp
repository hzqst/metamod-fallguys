/**
* vim: set ts=4 :
* =============================================================================
* SourceMod
* Copyright (C) 2004-2010 AlliedModders LLC.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, AlliedModders LLC gives you permission to link the
* code of this program (as well as its derivative works) to "Half-Life 2," the
* "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, AlliedModders LLC grants
* this exception to all derivative works.  AlliedModders LLC defines further
* exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
* or <http://www.sourcemod.net/license.php>.
*
* Version: $Id: detours.cpp 248 2008-08-27 00:56:22Z pred $
*/

#include "detours.h"
#include <asm/asm.h>

#define DETOUR_REGION_SIZE 0x1000

void *AllocatePageMemory(void *base, size_t size)
{
#ifdef _WIN32
	void* pv = VirtualAlloc((void*)NULL, DETOUR_REGION_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
	void* pv = mmap((void*)NULL, DETOUR_REGION_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
	return pv;
}

void SetReadWrite(void *base)
{
#ifdef _WIN32
	VirtualProtect(base, DETOUR_REGION_SIZE, PAGE_READWRITE, NULL);
#else
	mprotect(base, DETOUR_REGION_SIZE, PROT_READ | PROT_WRITE);
#endif
}

void SetReadExecute(void *base)
{
#ifdef _WIN32
	VirtualProtect(base, DETOUR_REGION_SIZE, PAGE_EXECUTE_READ, NULL);
#else
	mprotect(base, DETOUR_REGION_SIZE, PROT_READ | PROT_EXEC);
#endif
}

void FreePageMemory(void *base)
{
#ifdef _WIN32
	VirtualFree(base, 0, MEM_RELEASE);
#else
	munmap(base, DETOUR_REGION_SIZE);
#endif
}

// Push 64-bit value onto the stack using two instructions.
//
// Pushing 0xF00DF00DF00DF00D:
// push 0xF00DF00D
// mov [rsp+4], 0xF00DF00D
static inline void X64_Push_Imm64(JitWriter *jit, jit_int64_t val)
{
	jit->write_ubyte(IA32_PUSH_IMM32);
	jit->write_int32(jit_int32_t(val));
	if ((val >> 32) != 0)
		IA32_Mov_ESP_Disp8_Imm32(jit, 4, (val >> 32));
}

// Jump to absolute 64-bit address using multiple instructions.
//
// Jumping to address 0xF00DF00DF00DF00D:
// push 0xF00DF00D
// mov [rsp+4], 0xF00DF00D
// ret
static inline void X64_Jump_Abs(JitWriter *jit, void *dest)
{
	X64_Push_Imm64(jit, jit_int64_t(dest));
	IA32_Return(jit);
}

static inline void RelativeJump32(JitWriter *jit, void *target)
{
	jitoffs_t call = IA32_Jump_Imm32(jit, 0);
	IA32_Write_Jump32_Abs(jit, call, target);
}

#if defined(_WIN64) || defined(__x86_64__)
static inline bool IsShortJump(JitWriter *jit, void *target)
{
	int64_t diff = int64_t(target) - (int64_t(jit->outbase) + jit->get_outputpos() + OP_JMP_SIZE);
	int32_t upperBits = (diff >> 32);
	return upperBits == 0 || upperBits == -1;
}
#endif

static inline void AbsJump(JitWriter *jit, void *target)
{
#if defined(_WIN64) || defined(__x86_64__)
	if (IsShortJump(jit, target))
		RelativeJump32(jit, target);
	else
		X64_Jump_Abs(jit, target);
#else
	RelativeJump32(jit, target);
#endif
}

void CDetourManager::Init()
{

}

CDetour *CDetourManager::CreateDetour(void *callbackfunction, void **trampoline, void *pAddress)
{
	CDetour *detour = new CDetour(callbackfunction, trampoline, pAddress);
	if (detour)
	{
		if (!detour->Init())
		{
			delete detour;
			return NULL;
		}

		return detour;
	}

	return NULL;
}

CDetour::CDetour(void*callbackfunction, void **ptrampoline, void *pAddress)
{
	enabled = false;
	detoured = false;
	detour_address = pAddress;
	detour_trampoline = NULL;
	detour_callback = callbackfunction;
	trampoline = ptrampoline;
}

bool CDetour::Init()
{
	if (!CreateDetour())
	{
		enabled = false;
		return enabled;
	}

	enabled = true;

	return enabled;
}

void CDetour::Destroy()
{
	DeleteDetour();
	delete this;
}

bool CDetour::IsEnabled()
{
	return enabled;
}

bool CDetour::CreateDetour()
{
	if (!detour_address)
	{
		//g_pSM->LogError(myself, "Invalid function address passed for detour");
		return false;
	}
	
#if defined(_WIN64) || defined(__x86_64__)
	int shortBytes = copy_bytes((unsigned char *)detour_address, NULL, OP_JMP_SIZE);
	detour_restore.bytes = copy_bytes((unsigned char *)detour_address, NULL, X64_ABS_SIZE);
#else
	detour_restore.bytes = copy_bytes((unsigned char *)detour_address, NULL, OP_JMP_SIZE);
#endif

	JitWriter wr;
	JitWriter *jit = &wr;
	jit_uint32_t CodeSize = 0;

	wr.outbase = NULL;
	wr.outptr = NULL;

jit_rewind:

	/* Patch old bytes in */
	if (wr.outbase != NULL)
	{
#if defined(_WIN64) || defined(__x86_64__)
		wr.outptr += shortBytes;
		bool isShort = IsShortJump(jit, detour_address);
		wr.outptr -= shortBytes;
		if (isShort)
			detour_restore.bytes = shortBytes;
#endif
		/* Save restore bits */
		memcpy(detour_restore.patch, detour_address, detour_restore.bytes);

		copy_bytes((unsigned char *)detour_address, (unsigned char*)wr.outptr, detour_restore.bytes);
	}
	wr.outptr += detour_restore.bytes;

	/* Return to the original function */
	AbsJump(jit, (unsigned char *)detour_address + detour_restore.bytes);

	if (wr.outbase == NULL)
	{
		CodeSize = wr.get_outputpos();
		wr.outbase = (jitcode_t)AllocatePageMemory(detour_address, CodeSize);
		SetReadWrite(wr.outbase);
		wr.outptr = wr.outbase;
		detour_trampoline = wr.outbase;
		goto jit_rewind;
	}

	SetReadExecute(wr.outbase);

	*trampoline = detour_trampoline;

	return true;
}

void CDetour::DeleteDetour()
{
	if (detoured)
	{
		DisableDetour();
	}

	if (detour_trampoline)
	{
		/* Free the allocated trampoline memory */
		FreePageMemory(detour_trampoline);
		detour_trampoline = NULL;
	}
}

void CDetour::EnableDetour()
{
	if (!detoured)
	{
		DoGatePatch((unsigned char *)detour_address, detour_callback);
		detoured = true;
	}
}

void CDetour::DisableDetour()
{
	if (detoured)
	{
		/* Remove the patch */
		ApplyPatch(detour_address, 0, &detour_restore, NULL);
		detoured = false;
	}
}
