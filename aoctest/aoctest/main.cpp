#include <windows.h>
#include <stdio.h>
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

	if (AoC)
	{
		// Patch(0x00586F65 + 1, (BYTE)0x9C); // full_screen
		// Patch(0x00586F73 + 1, (BYTE)0x9C); // use_dir_draw
		// Patch(0x00586FDB + 8, (BYTE)0x00); // interface_style
		// Patch(0x00586F7A + 1, (BYTE)0xAC); // use_sys_mem
		// Patch(0x00586F6C + 1, (BYTE)0x9C);
		// Patch(0x00586E93 + 1, 768);
		// Patch(0x00586EB9 + 1, 1024);
	}
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return TRUE;
}
