#include <windows.h>
#include <stdint.h>
#include <assert.h>
typedef uintptr_t addr;
#include "..\aoctest\MemoryMgr.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

struct SlpHeader
{
	char version[4];
	int numFrames;
	char comment[24];
};

struct SlpFrame
{
	int cmdTableOffset;
	int outlineTableOffset;
	int paletteOffset;
	int properties;
	int width;
	int height;
	int centerX;
	int centerY;
};

struct TShape
{
	int alloctype;
	int size;
	int *frametbl;
	SlpHeader *slp;
	SlpFrame *frames;
	char isLoaded;
	int numFrames;

	TShape *ctor(char *name, int id);
	void dtor(void);
	TShape *btnctor(char *name, int id);
	void copy(void);
    inline short *getOutlineTable(int n) { return (short*)((char*)this->slp + this->frames[n].outlineTableOffset); }
	inline int *getCmdTable(int n) { return (int*)((char*)this->slp + this->frames[n].cmdTableOffset); }
	inline uchar *getCmd(int off) { return (uchar*)this->slp + off; }
};

WRAPPER TShape *TShape::ctor(char *name, int id) { EAXJMP(0x004DAE00); }
WRAPPER void TShape::dtor(void) { EAXJMP(0x004DB110); }
WRAPPER void *aoemalloc(int n) { EAXJMP(0x006146C3); }

int AoK, AoC, UserPatch;

uchar colmap[256];

void
ShapeForallPixels(TShape *slp, uchar (*colmap)[256])
{
	short *outline;
	int* cmdtable;
	uchar *cmd;
	uchar c;
	int npx;
	int y;
	SlpFrame *frame;
	int n;

	int gCurPlayerColor = 1;

	for (n = 0; n < slp->numFrames; n++) {
		frame = &slp->frames[n];
		outline = slp->getOutlineTable(n);
		cmdtable = slp->getCmdTable(n);
		cmd = slp->getCmd(*cmdtable);

		for (y = 0; y < frame->height; y++) {
			for (;;) {
				switch (*cmd & 0x003) {
				case 0x000:	/* small block */
					npx = *cmd++ >> 2;
					while (npx--) {
						*cmd = (*colmap)[*cmd];
						cmd++;
					}
					break;
				case 0x001:	/* small skip */
					npx = *cmd++ >> 2;
					break;
				default:
					switch (*cmd & 0x00F) {
					case 0x002:	/* block */
						npx = (int)(*cmd++ & 0x00F0) << 4;
						npx |= *cmd++;
						while (npx--) {
							*cmd = (*colmap)[*cmd];
							cmd++;
						}
						break;
					case 0x003:	/* skip */
						npx = (int)(*cmd++ & 0x00F0) << 4;
						npx |= *cmd++;
						break;
					case 0x006:	/* player block */
						npx = *cmd++ >> 4;
						if (npx == 0) npx = *cmd++;
						while (npx--) {
							cmd++;
						}
						break;
					case 0x007:	/* fill */
						npx = *cmd++ >> 4;
						if (npx == 0) npx = *cmd++;
						*cmd = (*colmap)[*cmd];
						cmd++;
						break;
					case 0x00A:	/* player fill */
						npx = *cmd++ >> 4;
						if (npx == 0) npx = *cmd++;
						c = *cmd++ + (gCurPlayerColor << 4);
						break;
					case 0x00B:	/* shadow fill ?? */
						npx = *cmd++ >> 4;
						if (npx == 0) npx = *cmd++;
						cmd++;
						break;
					case 0x00F:	/* end of line */
						cmd++;
						goto endofline;
					case 0x00E:	/* extended */
						switch (*cmd++ >> 4) {
						case 0x000:	/* next only if not flipped */
							break;
						case 0x001:	/* next only if flipped */
							break;
						case 0x002:	/* set normal color transform */
							break;
						case 0x003:	/* set alternate color transform */
							break;
						case 0x004:	/* one outline player */
							break;
						case 0x005:	/* outline player */
							cmd++;
							break;
						case 0x006:	/* one outline black */
							break;
						case 0x007:	/* outline black */
							cmd++;
							break;
						}
					}
					break;
				}
			}
		endofline:;
			outline += 2;
		}
	}
}

void
TShape::copy(void)
{
	uchar *data;
	int size;
	size = this->size;
	data = (uchar*)aoemalloc(size);
	assert(data);
	memcpy(data, this->slp, size);
	this->dtor();
	this->slp = (SlpHeader*)data;
	this->alloctype = 1;
	this->size = size;

	this->frames = (SlpFrame*)(this->slp+1);
	this->numFrames = this->slp->numFrames;
	this->isLoaded = 1;
}

TShape*
TShape::btnctor(char *name, int id)
{
	int i, n;
    TShape *shp;

    // fix color palette
	for (i = 0; i < 256; i++) {
		switch (i) {
			case 8:
				n = 5;
				break;
			case 12:
				n = 129;
				break;
			case 111:
				n = 106;
				break;
			case 142:
				n = 140;
				break;
			case 192:
				n = 249;
				break;
			case 212:
				n = 0;
				break;
			case 229:
				n = 23;
				break;
			case 248:
				n = 229;
				break;
			default:
				n = i;
		}

		colmap[i] = n;
	}

	shp = ctor(name, id);
	shp->copy(); // crashes otherwise
	ShapeForallPixels(shp, &colmap);

    return shp;
}

void
PatchButtons(void)
{
	Patch(0x00517062 + 1, 50753); // aok button slp
	InjectHook(0x0051706E, &TShape::btnctor);

	// button type
	Patch(0x0051765F + 1, (BYTE)6);
	Patch(0x00517721 + 1, (BYTE)6);
	Patch(0x005177E0 + 1, (BYTE)6);
	Patch(0x0051789F + 1, (BYTE)6);
	Patch(0x00517960 + 1, (BYTE)6);

	// AoC hardcoded resolutions
	// 1280
	Patch(0x0051A658 + 1, 1280 - 63);
	Patch(0x0051A650 + 1, (BYTE)17);
	Patch(0x0051A654 + 1, (BYTE)6);

	Patch(0x0051A642 + 1, 1280 - 122);
	Patch(0x0051A63A + 1, (BYTE)17);
	Patch(0x0051A63E + 1, (BYTE)6);

	Patch(0x0051A62C + 1, 1280 - 182);
	Patch(0x0051A626 + 1, (BYTE)17);
	Patch(0x0051A62A + 1, (BYTE)6);

	Patch(0x0051A616 + 1, 1280 - 242);
	Patch(0x0051A60E + 1, (BYTE)17);
	Patch(0x0051A612 + 1, (BYTE)6);

	Patch(0x0051A600 + 1, 1280 - 302);
	Patch(0x0051A5F8 + 1, (BYTE)17);
	Patch(0x0051A5FC + 1, (BYTE)6);

	// 1024
	Patch(0x0051A9C8 + 1, 1024 - 64);
	Patch(0x0051A9C0 + 1, (BYTE)17);
	Patch(0x0051A9C4 + 1, (BYTE)4);

	Patch(0x0051A9B2 + 1, 1024 - 123);
	Patch(0x0051A9AA + 1, (BYTE)17);
	Patch(0x0051A9AE + 1, (BYTE)4);

	Patch(0x0051A99C + 1, 1024 - 183);
	Patch(0x0051A996 + 1, (BYTE)17);
	Patch(0x0051A99A + 1, (BYTE)4);

	Patch(0x0051A986 + 1, 1024 - 243);
	Patch(0x0051A97E + 1, (BYTE)17);
	Patch(0x0051A982 + 1, (BYTE)4);

	Patch(0x0051A970 + 1, 1024 - 303);
	Patch(0x0051A968 + 1, (BYTE)17);
	Patch(0x0051A96C + 1, (BYTE)4);

	// 800
	Patch(0x0051AD38 + 1, 800 - 55);
	Patch(0x0051AD30 + 1, (BYTE)17);
	Patch(0x0051AD34 + 1, (BYTE)3);

	Patch(0x0051AD22 + 1, 800 - 114);
	Patch(0x0051AD1A + 1, (BYTE)17);
	Patch(0x0051AD1E + 1, (BYTE)3);

	Patch(0x0051AD0C + 1, 800 - 174);
	Patch(0x0051AD06 + 1, (BYTE)17);
	Patch(0x0051AD0A + 1, (BYTE)3);

	Patch(0x0051ACF6 + 1, 800 - 234);
	Patch(0x0051ACEE + 1, (BYTE)17);
	Patch(0x0051ACF2 + 1, (BYTE)3);

	Patch(0x0051ACE0 + 1, 800 - 294);
	Patch(0x0051ACD8 + 1, (BYTE)17);
	Patch(0x0051ACDC + 1, (BYTE)3);

	if (UserPatch)
	{
		Patch(0x007C14FB + 1, 4 + 60);
		Patch(0x007C14F2 + 1, (BYTE)17);

		Patch(0x007C14E1 + 1, 4 + 2 * 60);
		Patch(0x007C14D8 + 1, (BYTE)17);

		Patch(0x007C14C7 + 1, 4 + 3 * 60);
		Patch(0x007C14BE + 1, (BYTE)17);

		Patch(0x007C14AD + 1, 4 + 4 * 60);
		Patch(0x007C14A4 + 1, (BYTE)17);

		Patch(0x007C1493 + 1, 4 + 5 * 60);
		Patch(0x007C148A + 1, (BYTE)17);

		// and once more ... with footnotes

		Patch(0x007C1100 + 1, 4 + 60);
		Patch(0x007C10F7 + 1, (BYTE)17);

		Patch(0x007C10E6 + 1, 4 + 2 * 60);
		Patch(0x007C10DD + 1, (BYTE)17);

		Patch(0x007C10CC + 1, 4 + 3 * 60);
		Patch(0x007C10C3 + 1, (BYTE)17);

		Patch(0x007C10B2 + 1, 4 + 4 * 60);
		Patch(0x007C10A9 + 1, (BYTE)17);

		Patch(0x007C1098 + 1, 4 + 5 * 60);
		Patch(0x007C108F + 1, (BYTE)17);
	}
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
