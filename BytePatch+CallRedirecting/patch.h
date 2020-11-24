#pragma once
#include <windows.h>
#include <vector>

namespace patch
{
	void Patch(BYTE* dest, BYTE* src, unsigned int byteSize);

	void Nop(BYTE* dest, unsigned int byteSize);
}

namespace call
{
	void RedirectCall(WORD addrOffset, DWORD* _Out_ oldCall, void* newCall);
}