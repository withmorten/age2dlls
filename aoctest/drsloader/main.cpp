#include <windows.h>
#include <stdint.h>
#include <Shlwapi.h>
#include <assert.h>
typedef uintptr_t addr;
#include "..\aoctest\MemoryMgr.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

WRAPPER int RESOURCE_MANAGER_open_resource_file(char *filename, char *game, char *dirpath, int nomap) { EAXJMP(0x00543370); }

int AoK, AoC, UserPatch;

int
RESOURCE_MANAGER_open_resource_file_hook(char *filename, char *game, char *dirpath, int nomap)
{
	WIN32_FIND_DATA fd;
	HANDLE handle;
	char *ext, *p;

	handle = FindFirstFile("Mods\\drs\\*.drs", &fd);
	if (handle != INVALID_HANDLE_VALUE) {
		do
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				ext = strrchr(fd.cFileName, '.');
				for (p = ext; *p; p++) { *p = tolower(*p); }

				if (strcmp(ext, ".drs") == 0)
					RESOURCE_MANAGER_open_resource_file(fd.cFileName, game, "Mods\\drs\\", 0);
			}
		while (FindNextFile(handle, &fd));
		FindClose(handle);
	}

	return RESOURCE_MANAGER_open_resource_file(filename, game, dirpath, nomap);
}

extern "C"
__declspec(dllexport) void
Init(void)
{
	AoK = (*(int*)0x005FB0D3 == 0x042474FF);
	AoC = (*(int*)0x006146F0 == 0x042474FF);
	UserPatch = (AoC && *(int*)0x0051A3B8 == 0x002A6674);

	if(AoC)
		InjectHook(0x0043AF73, RESOURCE_MANAGER_open_resource_file_hook);
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return TRUE;
}
