#include <windows.h>
#include <stdio.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <stdint.h>
#include <assert.h>
typedef uintptr_t addr;
#include "MemoryMgr.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

struct ddraw_dll
{
	HMODULE dll;
	FARPROC AcquireDDThreadLock;
	FARPROC CompleteCreateSysmemSurface;
	FARPROC D3DParseUnknownCommand;
	FARPROC DDGetAttachedSurfaceLcl;
	FARPROC DDInternalLock;
	FARPROC DDInternalUnlock;
	FARPROC DSoundHelp;
	FARPROC DirectDrawCreate;
	FARPROC DirectDrawCreateClipper;
	FARPROC DirectDrawCreateEx;
	FARPROC DirectDrawEnumerateA;
	FARPROC DirectDrawEnumerateExA;
	FARPROC DirectDrawEnumerateExW;
	FARPROC DirectDrawEnumerateW;
	FARPROC DllCanUnloadNow;
	FARPROC DllGetClassObject;
	FARPROC GetDDSurfaceLocal;
	FARPROC GetOLEThunkData;
	FARPROC GetSurfaceFromDC;
	FARPROC RegisterSpecialCase;
	FARPROC ReleaseDDThreadLock;
	FARPROC SetAppCompatData;
} ddraw;

WRAPPER void _AcquireDDThreadLock() { VARJMP(ddraw.AcquireDDThreadLock); }
WRAPPER void _CompleteCreateSysmemSurface() { VARJMP(ddraw.CompleteCreateSysmemSurface); }
WRAPPER void _D3DParseUnknownCommand() { VARJMP(ddraw.D3DParseUnknownCommand); }
WRAPPER void _DDGetAttachedSurfaceLcl() { VARJMP(ddraw.DDGetAttachedSurfaceLcl); }
WRAPPER void _DDInternalLock() { VARJMP(ddraw.DDInternalLock); }
WRAPPER void _DDInternalUnlock() { VARJMP(ddraw.DDInternalUnlock); }
WRAPPER void _DSoundHelp() { VARJMP(ddraw.DSoundHelp); }
WRAPPER void _DirectDrawCreate() { VARJMP(ddraw.DirectDrawCreate); }
WRAPPER void _DirectDrawCreateClipper() { VARJMP(ddraw.DirectDrawCreateClipper); }
WRAPPER void _DirectDrawCreateEx() { VARJMP(ddraw.DirectDrawCreateEx); }
WRAPPER void _DirectDrawEnumerateA() { VARJMP(ddraw.DirectDrawEnumerateA); }
WRAPPER void _DirectDrawEnumerateExA() { VARJMP(ddraw.DirectDrawEnumerateExA); }
WRAPPER void _DirectDrawEnumerateExW() { VARJMP(ddraw.DirectDrawEnumerateExW); }
WRAPPER void _DirectDrawEnumerateW() { VARJMP(ddraw.DirectDrawEnumerateW); }
WRAPPER void _DllCanUnloadNow() { VARJMP(ddraw.DllCanUnloadNow); }
WRAPPER void _DllGetClassObject() { VARJMP(ddraw.DllGetClassObject); }
WRAPPER void _GetDDSurfaceLocal() { VARJMP(ddraw.GetDDSurfaceLocal); }
WRAPPER void _GetOLEThunkData() { VARJMP(ddraw.GetOLEThunkData); }
WRAPPER void _GetSurfaceFromDC() { VARJMP(ddraw.GetSurfaceFromDC); }
WRAPPER void _RegisterSpecialCase() { VARJMP(ddraw.RegisterSpecialCase); }
WRAPPER void _ReleaseDDThreadLock() { VARJMP(ddraw.ReleaseDDThreadLock); }
WRAPPER void _SetAppCompatData() { VARJMP(ddraw.SetAppCompatData); }

void
DDraw(HINSTANCE hInst)
{
	TCHAR DDrawDllPath[MAX_PATH];

	GetEnvironmentVariable("windir", DDrawDllPath, MAX_PATH);
	strcat_s(DDrawDllPath, "\\System32\\ddraw.dll");

	ddraw.dll = LoadLibrary(DDrawDllPath);

	ddraw.AcquireDDThreadLock = GetProcAddress(ddraw.dll, "AcquireDDThreadLock");
	ddraw.CompleteCreateSysmemSurface = GetProcAddress(ddraw.dll, "CompleteCreateSysmemSurface");
	ddraw.D3DParseUnknownCommand = GetProcAddress(ddraw.dll, "D3DParseUnknownCommand");
	ddraw.DDGetAttachedSurfaceLcl = GetProcAddress(ddraw.dll, "DDGetAttachedSurfaceLcl");
	ddraw.DDInternalLock = GetProcAddress(ddraw.dll, "DDInternalLock");
	ddraw.DDInternalUnlock = GetProcAddress(ddraw.dll, "DDInternalUnlock");
	ddraw.DSoundHelp = GetProcAddress(ddraw.dll, "DSoundHelp");
	ddraw.DirectDrawCreate = GetProcAddress(ddraw.dll, "DirectDrawCreate");
	ddraw.DirectDrawCreateClipper = GetProcAddress(ddraw.dll, "DirectDrawCreateClipper");
	ddraw.DirectDrawCreateEx = GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
	ddraw.DirectDrawEnumerateA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateA");
	ddraw.DirectDrawEnumerateExA = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExA");
	ddraw.DirectDrawEnumerateExW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateExW");
	ddraw.DirectDrawEnumerateW = GetProcAddress(ddraw.dll, "DirectDrawEnumerateW");
	ddraw.DllCanUnloadNow = GetProcAddress(ddraw.dll, "DllCanUnloadNow");
	ddraw.DllGetClassObject = GetProcAddress(ddraw.dll, "DllGetClassObject");
	ddraw.GetDDSurfaceLocal = GetProcAddress(ddraw.dll, "GetDDSurfaceLocal");
	ddraw.GetOLEThunkData = GetProcAddress(ddraw.dll, "GetOLEThunkData");
	ddraw.GetSurfaceFromDC = GetProcAddress(ddraw.dll, "GetSurfaceFromDC");
	ddraw.RegisterSpecialCase = GetProcAddress(ddraw.dll, "RegisterSpecialCase");
	ddraw.ReleaseDDThreadLock = GetProcAddress(ddraw.dll, "ReleaseDDThreadLock");
	ddraw.SetAppCompatData = GetProcAddress(ddraw.dll, "SetAppCompatData");
}

int AoK, AoC, UserPatch;

void
patchEula() {
	// patches the EULA confirmation so the game runs without EBEULA(X).DLL
	// mov al, 1; ret
	BYTE eulapatch[] = { 0xB0, 0x01, 0xC3 };

	if (AoK)
		PatchBytes(0x0047B8A0, eulapatch, sizeof(eulapatch));
	else if (AoC)
		PatchBytes(0x00587400, eulapatch, sizeof(eulapatch));
}

void
userPatchDlls(void)
{
	if (UserPatch) {
		const char *wndmode = "wndmode.dll";
		const char *miniupnpc = "miniupnpc.dll";
		const char *spectate = "spectate.exe";
		const char *WK = "WK";

		// wndmode.dll crashes under WinXP (and probably below) - so let's just not load it in that case
		if ((DWORD)(LOBYTE(LOWORD(GetVersion()))) < 6)
			Patch(0x007C2815, (BYTE)0xEB); // JNZ to JMP

		// if the game binary (and UP dlls) are in root folder (i.e. C:\Age of Empires II\age2_x1.exe)
		if (!PathFileExists("age2_x1\\age2_x1.exe")) {
			Patch(0x007C2817 + 1, wndmode);
			Patch(0x007DB0EC + 1, miniupnpc);
			Patch(0x007E65C6 + 1, spectate);
		}

		if (PathFileExists("Games\\WK.xml")) {
			Patch(0x007C2A60 + 1, WK);
		}
	}
}

void
loadDlhs(void)
{
	WIN32_FIND_DATA fd;
	HANDLE hFindFile;
	char *ext, *p;
	char dlhPath[MAX_PATH];
	HMODULE dllInst;
	void(*Init)();

	hFindFile = FindFirstFile("Mods\\*.dlh", &fd);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				ext = strrchr(fd.cFileName, '.');
				for (p = ext; *p; p++) { *p = tolower(*p); }

				if (strcmp(ext, ".dlh") == 0) {
					strcpy_s(dlhPath, "Mods\\");
					strcat_s(dlhPath, fd.cFileName);
				}

				dllInst = LoadLibrary(dlhPath);

				if (dllInst != NULL) {
					Init = (void(*)())GetProcAddress(dllInst, "Init");
				
					if (Init != NULL)
						(*Init)();
				}
			}
		} while (FindNextFile(hFindFile, &fd));
		FindClose(hFindFile);
	}
}

int (*WinMainOrig)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

int
WinMainHook(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	if (!PathFileExists("data\\graphics.drs"))
		SetCurrentDirectory("..\\");

	patchEula();
	userPatchDlls();
	loadDlhs();

	return WinMainOrig(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH) {
		DDraw(hInst);

		AoK = (*(int*)0x005FB0D3 == 0x00042474FF);
		AoC = (*(int*)0x006146F0 == 0x00042474FF);
		UserPatch = (AoC && *(int*)0x0051A3B8 == 0x002A6674);

		addr WinMainAddr;

		if (AoK)
			INTERCEPT(WinMainAddr, WinMainHook, 0x005FE48A);
		else if (AoC)
			INTERCEPT(WinMainAddr, WinMainHook, 0x006175EB);

		WinMainOrig = (int(*)(HINSTANCE, HINSTANCE, LPSTR, int))WinMainAddr;
	}

	if(reason == DLL_PROCESS_DETACH)
		FreeLibrary(ddraw.dll);

	return TRUE;
}
