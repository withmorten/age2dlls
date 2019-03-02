#include <windows.h>
#include <stdint.h>
#include <assert.h>
typedef uintptr_t addr;
#include "..\aoctest\MemoryMgr.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

int AoK, AoC, UserPatch;

char *open = "stream\\open.mp3";
char *town = "stream\\town.mp3";

ASM(banner_pushargs)
{
	__asm {
		mov	eax, [ecx]
		push	187
		push	125
		push	4
		push	0
		RET(0x0050BD4C);
	}
}

void
PatchButtons(void)
{
	Patch(0x0050B861 + 1, 50081); // main.sin
	Patch(0x0050B900 + 1, 50231); // main.slp
	Patch(0x0050E1B1 + 1, open);
	Patch(0x0050E1D8 + 1, town);

    // menu: x, y, width, height
	// single player
	Patch(0x0050BCE5 + 1, 306);
	Patch(0x0050BCE3 + 1, (BYTE)7);
	Patch(0x0050BCE1 + 1, (BYTE)125);
	Patch(0x0050BCDC + 1, 197);

	// multiplayer
	Patch(0x0050BD01 + 1, 258);
	Patch(0x0050BCFA + 1, 214);
	Patch(0x0050BCF8 + 1, (BYTE)105);
	Patch(0x0050BCF3 + 1, 136);

	// history
	Patch(0x0050BD1A + 1, (BYTE)100);
	Patch(0x0050BD13 + 1, 161);
	Patch(0x0050BD11 + 1, (BYTE)109);
	Patch(0x0050BD0F + 1, (BYTE)103);

	// editor
	Patch(0x0050BD30 + 1, 194);
	Patch(0x0050BD29 + 1, 271);
	Patch(0x0050BD27 + 1, (BYTE)76);
	Patch(0x0050BD25 + 1, (BYTE)72);

	// the banner, but apparently not used in AoK
	// InjectHook(0x0050BD3E, banner_pushargs, PATCH_JUMP);

	// exit
	Patch(0x50BD63 + 1, (BYTE)0);
	Patch(0x50BD5C + 1, 533);
	Patch(0x50BD57 + 1, 162);
	Patch(0x50BD55 + 1, (BYTE)67);

	// learn to play
	Patch(0x50BD75 + 1, (BYTE)4);

	// options
	Patch(0x50BD8E + 1, (BYTE)97);
	Patch(0x50BD87 + 1, 344);
	Patch(0x50BD85 + 1, (BYTE)100);
	Patch(0x50BD83 + 1, (BYTE)105);

	// the zone
	Patch(0x50BDA4 + 1, 268);
	Patch(0x50BD9D + 1, 364);
	Patch(0x50BD9B + 1, (BYTE)74);
	Patch(0x50BD99 + 1, (BYTE)70);
}

extern "C"
__declspec(dllexport) void
Init(void)
{
	AoK = (*(int*)0x005FB0D3 == 0x042474FF);
	AoC = (*(int*)0x006146F0 == 0x042474FF);
	UserPatch = (AoC && *(int*)0x0051A3B8 == 0x002A6674);

	if (AoC)
		PatchButtons();
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return TRUE;
}
