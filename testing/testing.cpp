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
	cout << "dwGameUpdate\t\t[0x" << (void *)(gdwGameUpdate - gdwBaseAddress) << "]" << endl;
	cout << "dwAddCollectible\t[0x" << (void *)(gdwAddCollectible - gdwBaseAddress) << "]" << endl;
	cout << "pPlayer\t\t\t[0x" << (void *)GetPlayerEntity() << "]" << endl;
	cout << "gdwPlayerManager\t[0x" << (void *)(gdwPlayerManager - gdwBaseAddress) << "]" << endl;
	cout << "pPlayerManager\t\t[0x" << (void *)GetPlayerManager() << "]" << endl;

}




DWORD gdwPlayer0000 = 0;
int (__fastcall *original_Player0000)(int self, int _EDX, int a2);
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

int(__fastcall *original_sub_552A10)(void *a1, int a2, char *, char a4, char a5);
int __fastcall sub_552A10(void *a1, int a2, char *a3, char a4, char a5)
{
	printarg("a1", (DWORD)a1);
	printarg("a2", a2);
	cout << "a3:\t" << a3 << endl;
	printarg("a4", a4);
	printarg("a5", a5);
	int ret = original_sub_552A10(a1, a2, "Testing", a4, a5);
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

DWORD original_FlashText = 0;
void* FlashTexts_retAddr = NULL;
DWORD _RET = 0;
DWORD _EAX = 0;
DWORD _EDI = 0;
DWORD _ESI = 0;
void __stdcall printer(int arg)
{
	printarg(1, arg);
}
__declspec(naked) char nFlashText()
{
	_asm
	{
		mov _EAX, eax
			push _EAX
			call printer
		mov _EDI, edi
		mov _ESI, esi
		jmp original_FlashText
		mov _RET, eax
	}
	printarg("_EAX", _EAX);
	printarg("_EDI", _EDI);
	printarg("_ESI", _ESI);
	printarg("_RET", _RET);
}



void testcall(int argc, char *argv[])
{
	DWORD arg1 = gdwBaseAddress + 0x1BC66C;
	printarg("arg1", arg1);
	DWORD arg12 = *(DWORD*)(gdwBaseAddress + 0x1BC66C);
	printarg("arg12", arg12);
	DWORD arg13 = (DWORD)(DWORD *)(gdwBaseAddress + 0x1BC66C);
	printarg("arg13", arg13);
	DWORD arg2 = (int)&GetPlayerManager()->_floorNo + 0x2A5A4;
	printarg("arg2", arg2);
	char *arg3 = argv[0];
	cout << "arg3:\t" << arg3 << endl;



	original_sub_552A10(&arg1, arg2, arg3, 0, 0);

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
	AddCommand("testcall", testcall);

	/*
	gdwPlayer0000 = gdwBaseAddress + 0xCD330;
	original_Player0000 = (int(__fastcall *)(int self, int _EDX, int a2))DetourFunction((PBYTE)gdwPlayer0000, (PBYTE)Player0000);

	gdwPlayer0004 = gdwBaseAddress + 0xCD4B0;
	original_Player0004 = (int(__fastcall *)(int self, int _EDX, int a2, int Args, unsigned int a4, int a5))DetourFunction((PBYTE)gdwPlayer0004, (PBYTE)Player0004);
	*/
	//if (gdwFlashText)
	//{
		//original_FlashText = (DWORD)DetourFunction((PBYTE)gdwFlashText, (PBYTE)nFlashText);
	//}

	DWORD gdwPlayer0000 = gdwBaseAddress + 0x4820;
	//original_Player0000 = (int(__fastcall *)(int self, int _EDX, int a2))DetourFunction((PBYTE)gdwPlayer0000, (PBYTE)Player0000);

	DWORD dwsub_552A10 = gdwBaseAddress + 0x152A10;
	original_sub_552A10 = (int(__fastcall *)(void *, int, char *, char, char))DetourFunction((PBYTE)dwsub_552A10, (PBYTE)sub_552A10);

}

// called when the plugin is removed
PAPI VOID UnInitPlugin(VOID)
{
	//DetourRemove((PBYTE)gdwPlayer0004, (PBYTE)original_Player0000);
	DetourRemove((PBYTE)original_Player0004, (PBYTE)Player0000);
	DetourRemove((PBYTE)original_sub_552A10, (PBYTE)sub_552A10);

	//remove commands, detours, etc
	RemoveCommand("spewvf");
	RemoveCommand("pills");
	RemoveCommand("setcurse");
	RemoveCommand("showoffsets");
	RemoveCommand("testcall");
	Sleep(1000);
}

PAPI VOID OnAddCollectible(Player *pPlayer, int relatedID, int itemID, int charges, int arg5)
{
}

DWORD dwFrameCount = 0;
PAPI VOID OnGameUpdate()
{
	//if you are going to do stuff in OnGameUpdate you should probably only do it once every 60 frames (1 sec at 60fps)
	if (dwFrameCount > 60 * 60)
	{
		//here is an example of doing something every 60*60 frames (once per minute)
		dwFrameCount = 0;
	}
	dwFrameCount++;
}