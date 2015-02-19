#include "statics.h"
#include "hooks.h"

PPLAYERMANAGER GetPlayerManager()
{
	return (PPLAYERMANAGER)*(DWORD*)gdwPlayerManager;
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

//int __usercall FlashText<eax>(const void *a1<eax>, size_t a2<edi>, int a3<esi>) // original
void FlashText(char *pChar)
{
	size_t len = strlen(pChar);
	__asm {
		pushad
			mov eax, pChar
			mov edi, len
			mov esi, 0x12
			jmp gdwFlashText
			pop eax
		popad
	}
}