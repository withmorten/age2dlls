#include <windows.h>
#include <stdint.h>
#include <assert.h>
typedef uintptr_t addr;
#include "..\aoctest\MemoryMgr.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

int AoK, AoC, UserPatch;

extern "C"
__declspec(dllexport) void
Init(void)
{
	AoK = (*(int*)0x005FB0D3 == 0x042474FF);
	AoC = (*(int*)0x006146F0 == 0x042474FF);
	UserPatch = (AoC && *(int*)0x0051A3B8 == 0x002A6674);

	if (AoC && UserPatch) {
		// Random Button, 0x0B is same font as most of menu but 0x0A looks better
		Patch<byte>(0x007AE806 + 1, 0x0A);

		// Reset Button
		Patch<byte>(0x007AF0B7 + 1, 0x0A);
	}
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return TRUE;
}
