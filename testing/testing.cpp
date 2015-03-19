#include "..\loader\plugin.h"
#include "..\loader\hooks.h"

#include <iostream>
#include <fstream>
#include <Psapi.h>

#define printarg(i, arg) cout << i << ":" << "\t" << (void *)arg << "\t" << (float)arg << endl;

typedef struct _PDWORDSTRUCT {
	union {
		unsigned long DWord[0x1];
		int Int[0x1];
		unsigned int UInt[0x1];
		float Float[0x1];
		struct _PDWORDSTRUCT *Void[0x1];
	};
} DWORDSTRUCT, *PDWORDSTRUCT;

class XSpew
{
public:
	PVOID vf;
};

//used to spew the virtual functions to .h and .idc files
void WriteLine(PCHAR FileName, PCHAR zFormat, ...)
{
	char Output[0x666];
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(Output, 0x666, zFormat, vaList);
	ofstream OutFile;
	OutFile.open(FileName, ios::app);
	OutFile << Output << "\n";
	OutFile.close();
}

void SearchVf(PVOID Search, PCHAR Class, DWORD DLLBase, DWORD Size, DWORD MemoryBase)
{
	if (!Search)
		return;
	if (Size > 0xfffff)
		return;
	char *pChar;
	char H[MAX_PATH] = { 0 };
	char IDC[MAX_PATH] = { 0 };
	char cMaxPath[MAX_PATH];
	char FileName[MAX_PATH];

	GetModuleFileNameA(NULL, FileName, sizeof(FileName));

	//Strip our file from the path.
	strcpy_s(cMaxPath, MAX_PATH, FileName);
	pChar = strrchr(cMaxPath, '\\');
	if (pChar)
		pChar[1] = 0;
	sprintf_s(H, MAX_PATH, "%s%s.h", cMaxPath, Class);
	sprintf_s(IDC, MAX_PATH, "%s%s.idc", cMaxPath, Class);
	WriteLine(H, "#ifndef _%s_H_\n#define _%s_H_\n\nclass %s\n{\npublic:", Class, Class, Class);
	WriteLine(IDC, "#define UNLOADED_FILE   1\n#include <idc.idc>\n\nstatic main(void) {\n");
	PDWORDSTRUCT pDWordStruct = (PDWORDSTRUCT)Search;
	for (DWORD i = 0; i < Size; i += 4)
	{
		if (pDWordStruct->DWord[i / 4])
		{
			WriteLine(H, "/*[0x%04X]*/\tvirtual void function%04X(); //", i, i);
			WriteLine(IDC, "MakeName(0x%X,\"%s_0x%04X\");", DLLBase + (pDWordStruct->DWord[i / 4] - MemoryBase), Class, i);
		}
	}
	WriteLine(H, "};\n\n#endif /*_%s_H_*/", Class);
	WriteLine(IDC, "}");
}

void SpewVF(int argc, char *argv[])
{
	printf("USAGE: SpewVf(location,dll,size,type,classname)\n\tlocation can be an offset of dll, or a memory location.\n\tdll is the name of the dll the function table will be located in\n\tsize is size of the vf table\n\ttype can be offset (for offset of dll) or memory\n\tclassname is what you want to name the class in .idc and .h files\n");
	if (argc != 5) return;
	char * pOffset, *pDll, *pSize, *blah, *pType, *pClass;
	XSpew *xPointer;
	pOffset = argv[0];
	pDll = argv[1];
	pSize = argv[2];
	pType = argv[3];
	pClass = argv[4];

	DWORD DLL = (DWORD)GetModuleHandleA(pDll);
	printf("DLLModuleHandle: 0x%X\n", DLL);
	if (!_stricmp(pType, "offset"))
	{
		DWORD dPointer = *(DWORD*)(DLL + strtoul(pOffset, &blah, 16));
		xPointer = (XSpew *)dPointer;
	}
	else if (!_stricmp(pType, "memory"))
	{
		xPointer = (XSpew *)strtoul(pOffset, &blah, 16);
	}
	else
	{
		return;
	}
	if (!xPointer) return;
	if (!xPointer->vf) return;
	printf("Spewing vf_table[%#X] with a size of [%s] in class [%#X] from [%s]\n", xPointer->vf, pSize, xPointer, pDll);
	SearchVf(xPointer->vf, pClass, 0x400000, strtoul(pSize, &blah, 16), DLL);
	return;
}


void setCurse(int argc, char *argv[])
{
	PlayerManager *pPlayerManager = GetPlayerManager();
	cout << "pPlayerManager [0x" << (void *)pPlayerManager << "]" << endl;
	pPlayerManager->i_curses = atoi(argv[0]);
}

int curPill = 0;
void Pills(int argc, char *argv[])
{
	GetPlayerEntity()->_pocket1ID = curPill;
	GetPlayerEntity()->_pocket2ID = curPill + 1;

	curPill++;
}

void showoffsets(int argc, char *argv[])
{
	cout << "gdwBaseAddress\t\t[0x" << (void *)gdwBaseAddress << "]" << endl;
	cout << "gdwBaseSize\t\t[0x" << (void *)gdwBaseSize << "]" << endl;
	cout << "gdwGameUpdate\t\t[0x" << (void *)(gdwGameUpdate - gdwBaseAddress) << "]" << endl;
	cout << "gdwPlayerManager\t[0x" << (void *)(gdwPlayerManager - gdwBaseAddress) << "]" << endl;
	cout << "pPlayerManager\t\t[0x" << (void *)GetPlayerManager() << "]" << endl;
	cout << "pPlayer\t\t\t[0x" << (void *)GetPlayerEntity() << "]" << endl;

}

float __stdcall blah(float fin)
{
	return floor(fin);
}

void showstats(int argc, char *argv[])
{
	Player *pPlayer = GetPlayerEntity();
}

DWORD ItemPool__GetCollectible_Original = 0;
int __stdcall ItemPool__GetCollectible_Payload(int a1)
{
	printarg('a1', a1);
	return a1;
}
__declspec(naked) void ItemPool__GetCollectible_Hook()
{
	_asm
	{
		jmp ItemPool__GetCollectible_Original
			push eax
			call ItemPool__GetCollectible_Payload
			pop eax
	}
}

DWORD gdwPlayer0000 = 0;
int(__fastcall *original_Player0000)(int self, int _EDX, int a2);
int  __fastcall Player0000(int self, int _EDX, int a2)
{
	printarg("self", self);
	printarg("EDX", _EDX);
	printarg("a2", a2);
	int ret = original_Player0000(self, _EDX, a2);
	printarg("ret", ret);
	cout << endl;
	return ret;
}


//for a one liner use second_line, two liner second_line and first_line, three liner third_line, second_line, and first_line in these orders!
int(__fastcall *original_show_fortune_banner)(void *pPlayerManager_plus_2A5A4, char *third_line, char *second_line, char *first_line);
int  __fastcall detour_show_fortune_banner(void *pPlayerManager_plus_2A5A4, char *third_line, char *second_line, char *first_line)
{
	printarg("pPlayerManager_plus_2A5A4", (int)pPlayerManager_plus_2A5A4);
	cout << third_line << endl;
	cout << second_line << endl;
	cout << first_line << endl;
	int ret = original_show_fortune_banner(pPlayerManager_plus_2A5A4, third_line, second_line, first_line);
	printarg("ret", ret);
	cout << endl;
	return ret;
}

int(__fastcall *original_sub_552A10)(char *lower_text, int PlayerManagerUnk, char *upper_text, bool show_lower_banner, bool is_bottom_banner);
int __fastcall sub_552A10(char *lower_text, int PlayerManagerUnk, char *upper_text, bool show_lower_banner, bool is_bottom_banner)
{

	void ** puEBP = NULL;
	__asm { mov puEBP, ebp };
	void * pvReturn = puEBP[1]; // this is the caller of my function

	puEBP = (void**)puEBP[0];    // walk back to the previous frame
	void * pvReturn2 = puEBP[1]; // this is the caller's caller
	cout << "sub_552A10 start()" << endl;
	printarg("pvReturn", ((int)pvReturn - (int)gdwBaseAddress));
	printarg("pvReturn2", ((int)pvReturn2 - (int)gdwBaseAddress));
	cout << "lower_text:\t" << lower_text << endl;
	printarg("PlayerManagerUnk", PlayerManagerUnk);
	cout << "upper_text:\t" << upper_text << endl;
	printarg("show_lower_banner", show_lower_banner);
	printarg("is_bottom_banner", is_bottom_banner);
	int ret = original_sub_552A10(lower_text, PlayerManagerUnk, "Testing", show_lower_banner, is_bottom_banner);
	printarg("ret", ret);
	cout << "sub_552A10 end()" << endl;
	return ret;
}

int(__stdcall *original_sub_4ACA20)(int a1);
int __stdcall sub_4ACA20(int a1)
{
	cout << "sub_4ACA20 start()" << endl;
	printarg("a1", a1);
	int ret = original_sub_4ACA20(a1);
	printarg("ret", ret);
	cout << "sub_4ACA20 end()" << endl;
	return ret;
}

int(__cdecl *original_sub_52E1F0)();
int sub_52E1F0()
{
	cout << "sub_52E1F0 start()" << endl;
	int ret = original_sub_52E1F0();
	printarg("ret", ret);
	cout << "sub_52E1F0 end()" << endl;
	return ret;
}

int(__fastcall *original_sub_552D10)(int a1, int _EDX, int a2);
int __fastcall sub_552D10(int a1, int _EDX, int a2)
{
	cout << "sub_552D10 start()" << endl;
	printarg("a1", a1);
	printarg("_EDX", _EDX);
	printarg("a2", a2);
	int ret = original_sub_552D10(a1, _EDX, a2);
	printarg("ret", ret);
	cout << "sub_552D10 end()" << endl;
	return ret;
}

int(__fastcall *original_sub_4C99D0)(int a1, int _EDX, bool a2);
int __fastcall sub_4C99D0(int a1, int _EDX, bool a2)
{
	cout << "sub_4C99D0 start()" << endl;
	printarg("a1", a1);
	printarg("_EDX", _EDX);
	printarg("a2", a2);
	int ret = original_sub_4C99D0(a1, _EDX, a2);
	printarg("ret", ret);
	cout << "sub_4C99D0 end()" << endl;
	return ret;
}


int(__stdcall *original_sub_4ACE00)(signed int a1, char a2, unsigned int a3, int a4, int a5, int a6, int a7, unsigned int a8, int a9);
int __stdcall sub_4ACE00(signed int a1, char a2, unsigned int a3, int a4, int a5, int a6, int a7, unsigned int a8, int a9)
{
	printarg("a1", a1);
	printarg("a2", a2);
	printarg("a3", a3);
	printarg("a4", a4);
	printarg("a5", a5);
	printarg("a6", a6);
	printarg("a7", a7);
	printarg("a8", a8);
	printarg("a9", a9);
	int ret = original_sub_4ACE00(a1, a2, a3, a4, a5, a6, a7, a8, a9);
	printarg("ret", ret);
	cout << endl;
	return ret;
}


DWORD gdwPlayer0004 = 0;
int(__fastcall *original_Player0004)(int self, int _EDX, int a2, int Args, unsigned int a4, int a5);
int __fastcall Player0004(int self, int _EDX, int a2, int Args, unsigned int a4, int a5)
{
	printarg("self", self);
	printarg("EDX", _EDX);
	printarg("a2", a2);
	printarg("Args", Args);
	printarg("a4", a4);
	printarg("a5", a5);
	int ret = original_Player0004(self, _EDX, a2, Args, a4, a5);
	printarg("ret", ret);
	cout << endl;
	return ret;
}

void testcall(int argc, char *argv[])
{
	/*
	char *arg1 = _strdup("test1");
	DWORD arg2 = (DWORD)GetPlayerManager() + 0x2A5A4;
	char *arg3 = _strdup("test2");
	bool arg4 = false;
	bool arg5 = false;
	int ret = sub_552A10(arg1, arg2, arg3, arg4, arg5);
	//original_sub_552A10(arg1, arg2, arg3, 0, 0);
	*/

	/*
	DWORD dwResource = (*(DWORD*)(gdwBaseAddress + 0x21BCFC));
	Resource * pResource = (Resource *)dwResource;
	printarg("dwResource", (DWORD)dwResource);
	printarg("pResource", (DWORD)pResource);

	for (int i = 0; i < 0x15A; i++)
	if (pResource->ItemList[i])
	if (pResource->ItemList[i]->_id != 0x38)
	pResource->ItemList[i]->unknown0x90 = 0xFFFFFFFF;
	*/
}

// called when the plugin initializes
PAPI VOID InitPlugin()
{
	//Add commands, detours, etc
	AddCommand("spewvf", SpewVF);
	AddCommand("pills", Pills);
	AddCommand("setcurse", setCurse);
	AddCommand("showoffsets", showoffsets);
	AddCommand("showstats", showstats);
	AddCommand("testcall", testcall);

}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{

	//remove commands, detours, etc
	RemoveCommand("spewvf");
	RemoveCommand("pills");
	RemoveCommand("setcurse");
	RemoveCommand("showoffsets");
	RemoveCommand("showstats"); 
	RemoveCommand("testcall");
	Sleep(1000);
}
