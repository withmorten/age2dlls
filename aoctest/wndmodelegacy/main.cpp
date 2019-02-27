#include <windows.h>
#include <stdint.h>
#include <Shlwapi.h>
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

	if (!UserPatch && (DWORD)(LOBYTE(LOWORD(GetVersion()))) > 5) {
		if (AoK || (AoC && !PathFileExists("age2_x1\\age2_x1.exe")))
			LoadLibrary("wndmode.dll");
		else if (AoC)
			LoadLibrary("age2_x1\\wndmode.dll");
	}
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return TRUE;
}
