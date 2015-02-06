#include "statics.h"
#include "hooks.h"

PPLAYER gpPlayer = 0;

PPLAYERMANAGER GetPlayerManager()
{
	return (PPLAYERMANAGER)*(DWORD*)gdwPlayerManager;
}

PPLAYER GetPlayer()
{
	return gpPlayer;
}