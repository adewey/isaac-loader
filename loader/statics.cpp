#include "statics.h"
#include "hooks.h"

Player *gpPlayer = 0;

PPLAYERMANAGER GetPlayerManager()
{
	return (PPLAYERMANAGER)*(DWORD*)gdwPlayerManager;
}

Player *GetPlayer()
{
	return gpPlayer;
}