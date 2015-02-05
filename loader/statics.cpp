#include "isaacdata.h"

PlayerManager *pPlayerManager;
Player* pPlayer;

PlayerManager *GetPlayerManager(){
	return pPlayerManager;
}

void SetPlayerManager(PlayerManager* pPlayerMgr){
	pPlayerManager = pPlayerMgr;
}

