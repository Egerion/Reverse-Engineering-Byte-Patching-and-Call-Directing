#include "pch.h"
#include "patch.h"



//lets you patch the necessary address with your desired bytes
void patch::Patch(BYTE* dest, BYTE* src, unsigned int byteSize)
{
	DWORD protect;
	VirtualProtect(dest, byteSize, PAGE_EXECUTE_READWRITE, &protect);
	memcpy(dest, src, byteSize);
	VirtualProtect(dest, byteSize, protect, &protect);

}

void patch::Nop(BYTE* dest, unsigned int byteSize)
{
	DWORD oldprotect;
	VirtualProtect(dest, byteSize, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dest, 0x90, byteSize);					//x90 means nop operation 
	VirtualProtect(dest, byteSize, oldprotect, &oldprotect);
}

void call::RedirectCall(WORD addrOffset, DWORD* _Out_ oldCall, void* newCall)
{
	DWORD  procBase = (DWORD)GetModuleHandle(NULL);

	DWORD* addrPtr = (DWORD*)(procBase + addrOffset + 1);
	DWORD jumpOffset = procBase + addrOffset + 5;
	DWORD rel = (DWORD)(newCall)-jumpOffset;
	*addrPtr = rel;
}

