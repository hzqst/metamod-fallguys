#include "asm.h"
#include <stdlib.h>
#include <capstone.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#ifndef WIN32
//#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define REG_EAX			0//B8
#define REG_ECX			1//B9
#define REG_EDX			2//BA
#define REG_EBX			3//BB
#define REG_EBP			5//BD
#define REG_ESI			6//BE
#define REG_EDI			7//BF

#define IA32_MOV_REG_IMM		0xB8	// encoding is +r <imm32>
#endif

/**
* Checks if a call to a fpic thunk has just been written into dest.
* If found replaces it with a direct mov that sets the required register to the value of pc.
*
* @param dest		Destination buffer where a call opcode + addr (5 bytes) has just been written.
* @param pc		The program counter value that needs to be set (usually the next address from the source).
* @noreturn
*/
void check_thunks(unsigned char *dest, unsigned char *pc)
{
#if defined(_WIN32) || defined(__x86_64__)
	return;
#else
	/* Step write address back 4 to the start of the function address */
	unsigned char *writeaddr = dest - 4;
	unsigned char *calloffset = *(unsigned char **)writeaddr;
	unsigned char *calladdr = (unsigned char *)(dest + (unsigned int)calloffset);

	/* Lookup name of function being called */
	if ((*calladdr == 0x8B) && (*(calladdr+2) == 0x24) && (*(calladdr+3) == 0xC3))
	{
		//a thunk maybe?
		char movByte = IA32_MOV_REG_IMM;

		/* Calculate the correct mov opcode */
		switch (*(calladdr+1))
		{
		case 0x04://eax
			{
				movByte += REG_EAX;
				break;
			}
		case 0x1C://ebx
			{
				movByte += REG_EBX;
				break;
			}
		case 0x0C://ecx
			{
				movByte += REG_ECX;
				break;
			}
		case 0x14://edx
			{
				movByte += REG_EDX;
				break;
			}
		case 0x2C://ebp
			{
				movByte += REG_EBP;
				break;
			}
		case 0x34://esi
			{
				movByte += REG_ESI;
				break;
			}
		case 0x3C://edi
			{
				movByte += REG_EDI;
				break;
			}
		default:
			{
				printf("Unknown thunk: %c\n", *(calladdr+1));
#ifndef NDEBUG
				abort();
#endif
				break;
			}
		}

		/* Move our write address back one to where the call opcode was */
		writeaddr--;


		/* Write our mov */
		*writeaddr = movByte;
		writeaddr++;

		/* Write the value - The provided program counter value */
		*(void **)writeaddr = (void *)pc;
		writeaddr += 4;
	}
#endif
}

int copy_bytes(unsigned char *func, unsigned char *dest, int required_len)
{
	int bytecount = 0;

	csh handle = 0;
	if (cs_open(CS_ARCH_X86, 
#if defined(_WIN64) || defined(__x86_64__)
		CS_MODE_64
#else
		CS_MODE_32
#endif
		, &handle) == CS_ERR_OK)
	{
		if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) == CS_ERR_OK)
		{
			while (bytecount < required_len)
			{
				cs_insn* insts = NULL;
				size_t count = 0;

				const uint8_t* addr = (uint8_t*)func;
				uint64_t vaddr = ((uint64_t)func);
				size_t size = 15;

#ifdef _WIN32
				bool accessable = !IsBadReadPtr(addr, size);
				if (accessable)

#else
				if (1)
#endif
				{
					count = cs_disasm(handle, addr, size, vaddr, 1, &insts);
					if (count)
					{
						unsigned int insn_len = insts->size;
						bytecount += insn_len;
						if (dest)
						{
							const uint8_t* opcode = insts->detail->x86.opcode;
							if ((opcode[0] & 0xFE) == 0xE8)	// Fix CALL/JMP offset
							{
								dest[0] = func[0];
								dest++; func++;
								if (insts->detail->x86.operands[0].size == 4)
								{
									*(int32_t*)dest = func + *(int32_t*)func - dest;
									check_thunks(dest + 4, func + 4);
									dest += sizeof(int32_t);
								}
								else
								{
									*(int16_t*)dest = func + *(int16_t*)func - dest;
									dest += sizeof(int16_t);
								}
								func--;
							}
							else
							{
								memcpy(dest, func, insn_len);
								dest += insn_len;
							}
						}

						func += insn_len;

						if (insts) {
							cs_free(insts, count);
							insts = NULL;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return bytecount;
}

//insert a specific JMP instruction at the given location
void inject_jmp(void* src, void* dest) {
	*(unsigned char*)src = OP_JMP;
	*(long*)((unsigned char*)src+1) = (long)((unsigned char*)dest - ((unsigned char*)src + OP_JMP_SIZE));
}

//fill a given block with NOPs
void fill_nop(void* src, unsigned int len) {
	unsigned char* src2 = (unsigned char*)src;
	while (len) {
		*src2++ = OP_NOP;
		--len;
	}
}

void* eval_jump(void* src) {
	unsigned char* addr = (unsigned char*)src;

	if (!addr) return 0;

	//import table jump
	if (addr[0] == OP_PREFIX && addr[1] == OP_JMP_SEG) {
		addr += 2;
		addr = *(unsigned char**)addr;
		//TODO: if addr points into the IAT
		return *(void**)addr;
	}

	//8bit offset
	else if (addr[0] == OP_JMP_BYTE) {
		addr = &addr[OP_JMP_BYTE_SIZE] + *(char*)&addr[1];
		//mangled 32bit jump?
		if (addr[0] == OP_JMP) {
			addr = addr + *(int*)&addr[1];
		}
		return addr;
	}
	/*
	//32bit offset
	else if (addr[0] == OP_JMP) {
		addr = &addr[OP_JMP_SIZE] + *(int*)&addr[1];
	}
	*/

	return addr;
}
/*
from ms detours package
static bool detour_is_imported(PBYTE pbCode, PBYTE pbAddress)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((PVOID)pbCode, &mbi, sizeof(mbi));
	__try {
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)mbi.AllocationBase;
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			return false;
		}

		PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader +
														  pDosHeader->e_lfanew);
		if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
			return false;
		}

		if (pbAddress >= ((PBYTE)pDosHeader +
						  pNtHeader->OptionalHeader
						  .DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress) &&
			pbAddress < ((PBYTE)pDosHeader +
						 pNtHeader->OptionalHeader
						 .DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress +
						 pNtHeader->OptionalHeader
						 .DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size)) {
			return true;
		}
		return false;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}
*/
