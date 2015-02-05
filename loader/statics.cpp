#include "statics.h"

PPLAYERMANAGER gpPlayerManager = 0;
PPLAYER gpPlayer = 0;

PPLAYERMANAGER GetPlayerManager()
{
	return gpPlayerManager;
}

PPLAYER GetPlayer()
{
	return gpPlayer;
}