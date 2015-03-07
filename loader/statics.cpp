#include "statics.h"
#include "hooks.h"


DWORD gdwPlayerManager = 0;
DWORD gdwGetPlayerEntity = 0;
DWORD gdwVersion = 0;
DWORD gdwCurseOfTheBlind = 0;


DWORD original_curse_of_the_blind = 0x90909090;
void ToggleCurseOfTheBlind()
{
	DWORD d, ds;
	DWORD temp = gdwCurseOfTheBlind; //save what is at that address
	VirtualProtect((LPVOID)gdwCurseOfTheBlind, 0x4, PAGE_EXECUTE_READWRITE, &d);
	*(DWORD*)gdwCurseOfTheBlind = original_curse_of_the_blind; //set address to whatever is in original (our value, or the value we saved if we have run this before)
	VirtualProtect((LPVOID)gdwCurseOfTheBlind, 0x4, d, &ds);
	original_curse_of_the_blind = temp; //set original to what used to be at the address
}


int(__fastcall *o_show_fortune_banner)(int *HUD, char const *third_line, char const *second_line, char const *first_line) = 0;
void show_fortune_banner(char *first_line, char *second_line, char *third_line)
{
	o_show_fortune_banner(&GetPlayerManager()->_HUD, third_line, second_line, first_line);
}

int(__fastcall *o_show_item_banner)(char const *second_line, int *HUD, char const *first_line, bool is_bottom_banner, bool style_second_line) = 0;
void show_item_banner(char *first_line, char *second_line, bool style_second_line, bool is_bottom_banner)
{
	o_show_item_banner(second_line, &GetPlayerManager()->_HUD, first_line, is_bottom_banner, style_second_line);
}

PlayerManager *GetPlayerManager()
{
	return (PlayerManager *)*(DWORD*)gdwPlayerManager;
}

Player *GetPlayerEntity()
{
	PlayerManager *pPlayerManager = GetPlayerManager();
	if (!pPlayerManager)
		return NULL;
	Player *pPlayer = 0;
	__asm {
		pushad
			mov esi, pPlayerManager
			call gdwGetPlayerEntity
			mov pPlayer, eax 
		popad
	}
	return pPlayer;
}

char *GetIsaacVersion()
{
	return (char *)gdwVersion;
}

void InitStatics()
{
	gdwPlayerManager = *(DWORD*)(dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(BYTE*)"\x8b\x0d\x00\x00\x00\x00\x8b\x81\x30\x8a\x00\x00\x2b\x81\x2c\x8a\x00\x00\xc1\xf8\x02\xc3",
		"xx????xx????xx????xxxx") + 2);

	gdwGetPlayerEntity = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x8B\x86\x00\x00\x00\x00\x2B\x86\x00\x00\x00\x00\xA9\x00\x00\x00\x00\x75\x0F\x68\x00"
		"\x00\x00\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x08\x8B\x8E\x00\x00\x00\x00\x2B\x8E"
		"\x00\x00\x00\x00\xC1\xF9\x02\x3B\xF9\x73\x0C",
		"xx????xx????x????xxx????xxx????xxxxx????xx????xxxxxxx");

	gdwVersion = *(DWORD*)(dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x68\x00\x00\x00\x00\xC6\x05\x00\x00\x00\x00\x00\xE8",
		"x????xx?????x") + 1);

	DWORD dw_show_fortune_banner = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00"
		"\x50\x83\xEC\x54\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x53\x56\x57\x50"
		"\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\x8B\x45\x08",
		"xxxxxx????xx????xxxxx????xxxxxxxxxxxxxx????xxx");
	if (dw_show_fortune_banner)
		o_show_fortune_banner = (int(__fastcall *)(int *HUD, char const *third_line, char const *second_line, char const *first_line))dw_show_fortune_banner;

	DWORD dw_show_item_banner = dwFindPattern(gdwBaseAddress, gdwBaseSize,
		(PBYTE)"\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00"
		"\x00\x50\x83\xEC\x50\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x53\x56"
		"\x57\x50\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\x8B\x45\x08\x89\x45\xA8",
		"xxxxxx????xx????xxxxx????xxxxxxxxxxxxxx????xxxxxx");
	if (dw_show_item_banner)
		o_show_item_banner = (int(__fastcall *)(char const *lower_text, int *HUD, char const *upper_text, bool is_bottom_banner, bool show_lower_banner))dw_show_item_banner;

	/* curse of the blind setter */
	gdwCurseOfTheBlind = dwFindPattern(gdwBaseAddress, gdwBaseSize, (PBYTE)"\x83\x4E\x08\x40\x68", "xxxxx");
}
