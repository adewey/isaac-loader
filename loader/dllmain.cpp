#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <curl.h>
#include "log.h"
#include <windows.h>
#include <tlhelp32.h>
#include "detours.h"

INITIALIZE_EASYLOGGINGPP

#define getECX(p) _asm {mov p, ECX}
#define setECX(p) _asm {mov ECX, p}
#define getEBX(p) _asm {mov p, EBX}
#define setEBX(p) _asm {mov EBX, p}

#define MB(a) MessageBoxA(NULL, #a, #a, NULL)

bool gbAttached = false;
HANDLE ghThread;

char* gIsaacUrl = "http://www.isaactracker.com";
char* gTrackerID = "64a3c29a-c71b-4b35-b08c-38d3f5a70586";

unsigned long GetModuleSize(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseSize;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}


unsigned long GetModuleBase(unsigned long dwPID, wchar_t* pszModuleName, unsigned long* pdwSize)
{
	unsigned long dwResult;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hSnapshot)
	{
		MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
		if (Module32First(hSnapshot, &moduleEntry))
		{
			do
			{
				if (wcscmp(moduleEntry.szModule, pszModuleName) == 0)
				{
					dwResult = (unsigned long)moduleEntry.modBaseAddr;
					if (pdwSize)
						*pdwSize = moduleEntry.modBaseSize;
					break;
				}
			} while (Module32Next(hSnapshot, &moduleEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}


unsigned long GetProcessId(wchar_t* pszProcessName)
{
	unsigned long dwResult = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
		if (Process32First(hSnapshot, &processEntry))
		{
			do
			{
				if (wcscmp(processEntry.szExeFile, pszProcessName) == 0)
				{
					dwResult = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &processEntry));
		}
		if (hSnapshot)
		{
			CloseHandle(hSnapshot);
			hSnapshot = NULL;
		}
	}
	return dwResult;
}

bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;
	return (*szMask) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (DWORD)(dwAddress + i);
	return 0;
}


bool(__cdecl* original_checkForGoldenKey)();
bool checkForGoldenKey()
{
	LOG(INFO) << "checkForGoldenKey() Called";
	return true;
}

#pragma pack(1)
struct Entity
{
	char _unk0000[0xC];
	int _id;
	unsigned int _variant;
	int _subtype;
	char _unk0018[0x10];
	void* Paralysis;
	char _unk0021[0x48];
	int _tearType;
	char _unk0020[0x23C];
	float _scaleX;
	float _scaleY;
	char _unk000D[0x88];
	float dmg;  // no
	float dmg2; // no
	char _unk000C[0xC8];
	float x;
	float y;
	char _unk0[0x350];
};

#pragma pack(1)
struct Player : Entity
{
	char _unk76C[0x3E4];
	int _maxHearts;
	int _hearts;
	int _eternalHearts;
	int _soulHearts;
	int _blackHeartMask;
	int _jarHearts;
	int _numKeys;
	int _hasGoldenKey;
	int _numBombs;
	int _numCoins;
	char _unk900[0x6C];
	float _shotspeed;
	char _unkA00[0x08];
	float _damage;
	float _range;
	char _unkB00[0xBC];
	float _speed;
	float _luck;
	char _unkC00[0xB4];
	BOOL _items[0x15A];
	char _unkB78[0x1D88];
};

DWORD WINAPI updateServer(void* pThreadArgument)
{
	try
	{
		Player* pPlayer = ((Player *)pThreadArgument);
		/* craft our json object to send to the server */
		char buffer1[1024] = { 0 };
		strcat_s(buffer1, 512, "[");
		char itemidbuffer[0x8];
		for (int i = 0; i < 0x15A; i++)
		{
			ZeroMemory(itemidbuffer, 0x8);
			if (pPlayer->_items[i])
			{
				sprintf_s(itemidbuffer, 0x8, "%d,", i + 1);
				strcat_s(buffer1, 1024, itemidbuffer);
			}
		}
		/* replace our trailing comma with a closing bracket */
		buffer1[strlen(buffer1) - 1] = ']';
		char buffer2[2048] = { 0 };
		sprintf_s(buffer2, 2048, "{\"coins\": %d, \"bombs\": %d, \"keys\": %d, \"items\": %s}", pPlayer->_numCoins, pPlayer->_numBombs, pPlayer->_numKeys, buffer1);

		CURL *curl;
		char finalUrl[256] = { 0 };
		sprintf_s(finalUrl, 256, "%s/api/%s/pickup/", gIsaacUrl, gTrackerID);
		curl = curl_easy_init();
		if (curl) {
			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "Accept: application/json");
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer2);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(curl, CURLOPT_URL, finalUrl);
			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
	}
	catch (int){}
	return 0;
}

//unneeded for client side stuff
char* cpGetItems(int itemID)
{
	std::map<int, char*> items;
	items[1] = "The Sad Onion";
	items[2] = "The Inner Eye";
	items[3] = "Spoon Bender";
	items[4] = "Cricket's Head";
	items[5] = "My Reflection";
	items[6] = "Number One";
	items[7] = "Blood of the Martyr";
	items[8] = "Brother Bobby";
	items[9] = "Skatole";
	items[10] = "Halo of Flies";
	items[11] = "1up!";
	items[12] = "Magic Mushroom";
	items[13] = "The Virus";
	items[14] = "Roid Rage";
	items[15] = "<3";
	items[16] = "Raw Liver";
	items[17] = "Skeleton Key";
	items[18] = "A Dollar";
	items[19] = "Boom!";
	items[20] = "Transcendence";
	items[21] = "The Compass";
	items[22] = "Lunch";
	items[23] = "Dinner";
	items[24] = "Dessert";
	items[25] = "Breakfast";
	items[26] = "Rotten Meat";
	items[27] = "Wooden Spoon";
	items[28] = "The Belt";
	items[29] = "Mom's Underwear";
	items[30] = "Mom's Heels";
	items[31] = "Mom's Lipstick";
	items[32] = "Wire Coat Hanger";
	items[33] = "The Bible";
	items[34] = "The Book of Belial";
	items[35] = "The Necronomicon";
	items[36] = "The Poop";
	items[37] = "Mr. Boom";
	items[38] = "Tammy's Head";
	items[39] = "Mom's Bra";
	items[40] = "Kamikaze!";
	items[41] = "Mom's Pad";
	items[42] = "Bob's Rotten Head";
	items[43] = "Pills here!";
	items[44] = "Teleport!";
	items[45] = "Yum Heart";
	items[46] = "Lucky Foot";
	items[47] = "Doctor's Remote";
	items[48] = "Cupid's Arrow";
	items[49] = "Shoop da Whoop!";
	items[50] = "Steven";
	items[51] = "Pentagram";
	items[52] = "Dr. Fetus";
	items[53] = "Magneto";
	items[54] = "Treasure Map";
	items[55] = "Mom's Eye";
	items[56] = "Lemon Mishap";
	items[57] = "Distant Admiration";
	items[58] = "Book of Shadows";
	items[60] = "The Ladder";
	items[61] = "Tarot Card";
	items[62] = "Charm of the Vampire";
	items[63] = "The Battery";
	items[64] = "Steam Sale";
	items[65] = "Anarchist Cookbook";
	items[66] = "The Hourglass";
	items[67] = "Sister Maggy";
	items[68] = "Technology";
	items[69] = "Chocolate Milk";
	items[70] = "Growth Hormones";
	items[71] = "Mini Mush";
	items[72] = "Rosary";
	items[73] = "Cube of Meat";
	items[74] = "A Quarter";
	items[75] = "PHD";
	items[76] = "X-Ray Vision";
	items[77] = "My Little Unicorn";
	items[78] = "Book of Revelations";
	items[79] = "The Mark";
	items[80] = "The Pact";
	items[81] = "Dead Cat";
	items[82] = "Lord of the Pit";
	items[83] = "The Nail";
	items[84] = "We Need To Go Deeper!";
	items[85] = "Deck of Cards";
	items[86] = "Monstro's Tooth";
	items[87] = "Loki's Horns";
	items[88] = "Little Chubby";
	items[89] = "Spider Bite";
	items[90] = "The Small Rock";
	items[91] = "Spelunker Hat";
	items[92] = "Super Bandage";
	items[93] = "The Gamekid";
	items[94] = "Sack of Pennies";
	items[95] = "Robo-Baby";
	items[96] = "Little C.H.A.D.";
	items[97] = "The Book of Sin";
	items[98] = "The Relic";
	items[99] = "Little Gish";
	items[100] = "Little Steven";
	items[101] = "The Halo";
	items[102] = "Mom's Bottle of Pills";
	items[103] = "The Common Cold";
	items[104] = "The Parasite";
	items[105] = "The D6";
	items[106] = "Mr. Mega";
	items[107] = "The Pinking Shears";
	items[108] = "The Wafer";
	items[109] = "Money = Power";
	items[110] = "Mom's Contacts";
	items[111] = "The Bean";
	items[112] = "Guardian Angel";
	items[113] = "Demon Baby";
	items[114] = "Mom's Knife";
	items[115] = "Ouija Board";
	items[116] = "9 Volt";
	items[117] = "Dead Bird";
	items[118] = "Brimstone";
	items[119] = "Blood Bag";
	items[120] = "Odd Mushroom";
	items[121] = "Odd Mushroom";
	items[122] = "Whore of Babylon";
	items[123] = "Monster Manual";
	items[124] = "Dead Sea Scrolls";
	items[125] = "Bobby-Bomb";
	items[126] = "Razor Blade";
	items[127] = "Forget Me Now";
	items[128] = "Forever alone";
	items[129] = "Bucket of Lard";
	items[130] = "A Pony";
	items[131] = "Bomb Bag";
	items[132] = "A Lump of Coal";
	items[133] = "Guppy's Paw";
	items[134] = "Guppy's Tail";
	items[135] = "IV Bag";
	items[136] = "Best Friend";
	items[137] = "Remote Detonator";
	items[138] = "Stigmata";
	items[139] = "Mom's Purse";
	items[140] = "Bobs Curse";
	items[141] = "Pageant Boy";
	items[142] = "Scapular";
	items[143] = "Speed Ball";
	items[144] = "Bum Friend";
	items[145] = "Guppy's Head";
	items[146] = "Prayer Card";
	items[147] = "Notched Axe";
	items[148] = "Infestation";
	items[149] = "Ipecac";
	items[150] = "Tough Love";
	items[151] = "The Mulligan";
	items[152] = "Technology 2";
	items[153] = "Mutant Spider";
	items[154] = "Chemical Peel";
	items[155] = "The Peeper";
	items[156] = "Habit";
	items[157] = "Bloody Lust";
	items[158] = "Crystal Ball";
	items[159] = "Spirit of the Night";
	items[160] = "Crack the Sky";
	items[161] = "Ankh";
	items[162] = "Celtic Cross";
	items[163] = "Ghost Baby";
	items[164] = "The Candle";
	items[165] = "Cat-o-nine-tails";
	items[166] = "D20";
	items[167] = "Harlequin Baby";
	items[168] = "Epic Fetus";
	items[169] = "Polyphemus";
	items[170] = "Daddy Longlegs";
	items[171] = "Spider Butt";
	items[172] = "Sacrificial Dagger";
	items[173] = "Mitre";
	items[174] = "Rainbow Baby";
	items[175] = "Dad's Key";
	items[176] = "Stem Cells";
	items[177] = "Portable Slot";
	items[178] = "Holy Water";
	items[179] = "Fate";
	items[180] = "The Black Bean";
	items[181] = "White Pony";
	items[182] = "Sacred Heart";
	items[183] = "Tooth Picks";
	items[184] = "Holy Grail";
	items[185] = "Dead Dove";
	items[186] = "Blood Rights";
	items[187] = "Guppy's Hairball";
	items[188] = "Abel";
	items[189] = "SMB Super Fan";
	items[190] = "Pyro";
	items[191] = "3 Dollar Bill";
	items[192] = "Telepathy For Dummies";
	items[193] = "MEAT!";
	items[194] = "Magic 8 Ball";
	items[195] = "Mom's Coin Purse";
	items[196] = "Squeezy";
	items[197] = "Jesus Juice";
	items[198] = "Box";
	items[199] = "Mom's Key";
	items[200] = "Mom's Eyeshadow";
	items[201] = "Iron Bar";
	items[202] = "Midas' Touch";
	items[203] = "Humbleing Bundle";
	items[204] = "Fanny Pack";
	items[205] = "Sharp Plug";
	items[206] = "Guillotine";
	items[207] = "Ball of Bandages";
	items[208] = "Champion Belt";
	items[209] = "Butt Bombs";
	items[210] = "Gnawed Leaf";
	items[211] = "Spiderbaby";
	items[212] = "Guppy's Collar";
	items[213] = "Lost Contact";
	items[214] = "Anemic";
	items[215] = "Goat Head";
	items[216] = "Ceremonial Robes";
	items[217] = "Mom's Wig";
	items[218] = "Placenta";
	items[219] = "Old Bandage";
	items[220] = "Sad Bombs";
	items[221] = "Rubber Cement";
	items[222] = "Anti-Gravity";
	items[223] = "Pyromaniac";
	items[224] = "Cricket's Body";
	items[225] = "Gimpy";
	items[226] = "Black Lotus";
	items[227] = "Piggy Bank";
	items[228] = "Mom's Perfume";
	items[229] = "Monstro's Lung";
	items[230] = "Abaddon";
	items[231] = "Ball of Tar";
	items[232] = "Stop Watch";
	items[233] = "Tiny Planet";
	items[234] = "Infestation 2";
	items[236] = "E. Coli";
	items[237] = "Death's Touch";
	items[238] = "Key Piece 1";
	items[239] = "Key Piece 2";
	items[240] = "Experimental Treatment";
	items[241] = "Contract from Below";
	items[242] = "Infamy";
	items[243] = "Trinity Shield";
	items[244] = "Tech.5";
	items[245] = "20/20";
	items[246] = "Blue Map";
	items[247] = "BFFS!";
	items[248] = "Hive Mind";
	items[249] = "There's Options";
	items[250] = "BOGO Bombs";
	items[251] = "Starter Deck";
	items[252] = "Little Baggy";
	items[253] = "Magic Scab";
	items[254] = "Blood Clot";
	items[255] = "Screw";
	items[256] = "Hot Bombs";
	items[257] = "Fire Mind";
	items[258] = "Missing No.";
	items[259] = "Dark Matter";
	items[260] = "Black Candle";
	items[261] = "Proptosis";
	items[262] = "Missing Page 2";
	items[264] = "Smart Fly";
	items[265] = "Dry Baby";
	items[266] = "Juicy Sack";
	items[267] = "Robo-Baby 2.0";
	items[268] = "Rotten Baby";
	items[269] = "Headless Baby";
	items[270] = "Leech";
	items[271] = "Mystery Sack";
	items[272] = "BBF";
	items[273] = "Bob's Brain";
	items[274] = "Best Bud";
	items[275] = "Lil' Brimstone";
	items[276] = "Isaac's Heart";
	items[277] = "Lil' Haunt";
	items[278] = "Dark Bum";
	items[279] = "Big Fan";
	items[280] = "Sissy Longlegs";
	items[281] = "Punching Bag";
	items[282] = "How to Jump";
	items[283] = "D100";
	items[284] = "D4";
	items[285] = "D10";
	items[286] = "Blank Card";
	items[287] = "Book of Secrets";
	items[288] = "Box of Spiders";
	items[289] = "Red Candle";
	items[290] = "The Jar";
	items[291] = "Flush!";
	items[292] = "Satanic Bible";
	items[293] = "Head of Krampus";
	items[294] = "Butter Bean";
	items[295] = "Magic Fingers";
	items[296] = "Converter";
	items[297] = "Pandora's Box";
	items[298] = "Unicorn Stump";
	items[299] = "Taurus";
	items[300] = "Aries";
	items[301] = "Cancer";
	items[302] = "Leo";
	items[303] = "Virgo";
	items[304] = "Libra";
	items[305] = "Scorpio";
	items[306] = "Sagittarius";
	items[307] = "Capricorn";
	items[308] = "Aquarius";
	items[309] = "Pisces";
	items[310] = "Eve's Mascara";
	items[311] = "Judas' Shadow";
	items[312] = "Maggy's Bow";
	items[313] = "Holy Mantle";
	items[314] = "Thunder Thighs";
	items[315] = "Strange Attractor";
	items[316] = "Cursed Eye";
	items[317] = "Mysterious Liquid";
	items[318] = "Gemini";
	items[319] = "Cain's Other Eye";
	items[320] = "???'s Only Friend";
	items[321] = "Samson's Chains";
	items[322] = "Mongo Baby";
	items[323] = "Isaac's Tears";
	items[324] = "Undefined";
	items[325] = "Scissors";
	items[326] = "Breath of Life";
	items[327] = "The Polaroid";
	items[328] = "The Negative";
	items[329] = "The Ludovico Technique";
	items[330] = "Soy Milk";
	items[331] = "Godhead";
	items[332] = "Lazarus' Rags";
	items[333] = "The Mind";
	items[334] = "The Body";
	items[335] = "The Soul";
	items[336] = "Dead Onion";
	items[337] = "Broken Watch";
	items[338] = "The Boomerang";
	items[339] = "Safety Pin";
	items[340] = "Caffeine Pill";
	items[341] = "Torn Photo";
	items[342] = "Blue Cap";
	items[343] = "Latch Key";
	items[344] = "Match Book";
	items[345] = "Synthoil";
	items[346] = "A Snack";
	return items[itemID];
}

Player* gpPlayer = NULL;
int(__fastcall *original_itemPickup)(Player* pPlayer, int relatedID, int itemID, int charges, int arg5);
int __fastcall itemPickup(Player* pPlayer, int relatedID, int itemID, int charges, int arg5)
{
	LOG(INFO) << "dwItemPickup() called";
	LOG(INFO) << "pPlayer [0x" << pPlayer << "]";
	//store our player pointer if we dont have it
	if (gpPlayer != pPlayer)
		gpPlayer = pPlayer;

	if (relatedID != itemID)
		LOG(INFO) << "Started with item (" << cpGetItems(itemID) << ") [" << itemID << "]";
	else
		LOG(INFO) << "Picked up item (" << cpGetItems(itemID) << ") [" << itemID << "]";
	LOG(INFO) << "charges: [" << charges << "]";
	LOG(INFO) << "arg5 [" << arg5 << "] When isn't this 1? possibly 1 if fully charged??";
	int ret = original_itemPickup(pPlayer, relatedID, itemID, charges, arg5);
	/* update server! */
	CreateThread(NULL, 0, updateServer, pPlayer, 0L, NULL);
	return ret;
}




DWORD WINAPI DllThread(void* pThreadArgument)
{
	/* init curl */
	curl_global_init(CURL_GLOBAL_ALL);
	/* scan for functions */
	unsigned long dwPid = GetProcessId(L"isaac-ng.exe");
	unsigned long dwSize = 0;
	unsigned long dwBase = GetModuleBase(dwPid, L"isaac-ng.exe", &dwSize);
	LOG(INFO) << "dwBase [0x" << (void *)dwBase << "]";
	//DWORD dwCheckForGoldenKey = dwFindPattern(dwBase, dwSize, (BYTE*)"\x80\xB9\x68\x0B\x00\x00\x00\x75\x11\x8B\x81\x64\x0B\x00\x00\x85\xC0\x7E\x0A\x48\x89\x81\x64\x0B\x00\x00\xB0\x01\xC3\x32\xC0\xC3", "xx????xxxxx????xxxxxxx????xxxxxx");

	DWORD dwItemPickup = dwFindPattern(dwBase, dwSize,
		(BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50"
		"\x83\xEC\x78\xA1\x00\x00\x00\x00\x33\xC4\x89\x44\x24\x70\x53\x56\x57\xA1\x00\x00"
		"\x00\x00\x33\xC4\x50\x8D\x84\x24\x88\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\x75"
		"\x08\x8B\xD9\x89\x5C\x24\x1C\x89\x74\x24\x24\x81\xFE",
		"xxxxxxxxx????xx????xxxxx????xxxxxxxxxx????xxxxxxxxxx????xxxxxxxxxxxx");


	if (dwItemPickup)
	{
		char buffer[256] = { 0 };
		sprintf_s(buffer, 256, "0x%X", dwItemPickup - dwBase);
		LOG(INFO) << "dwItemPickup found [" << buffer << "]";
		original_itemPickup = (int(__fastcall *)(Player*, int, int, int, int))DetourFunction((PBYTE)dwItemPickup, (PBYTE)itemPickup);
	}

	/* wait to be detached */
	while (gbAttached){ Sleep(100); }
	/* un-detour functions */

	DetourRemove((PBYTE)original_itemPickup, (PBYTE)itemPickup);
	return 0;
}


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		/* use a global attached variable and a thread to make sure we don't crash if isaac is unloaded from under us */
		if (!gbAttached)
		{
			LOG(INFO) << "DLL Attached!";
			gbAttached = true;
			//everything should be done in a thread instead of as part of the process attach call 
			ghThread = CreateThread(NULL, 0, DllThread, NULL, 0L, NULL);
		}
		break;
	case DLL_PROCESS_DETACH:
		LOG(INFO) << "DLL Detached!";
		gbAttached = false;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}