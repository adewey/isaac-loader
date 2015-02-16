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