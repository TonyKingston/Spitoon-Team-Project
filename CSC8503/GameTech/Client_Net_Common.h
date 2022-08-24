#pragma once
#ifdef _x64
#include <cstdint>

#define NCL_Network
#include "Network_Code.h"
#endif _x64

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
};

struct sPlayerDescription
{
	uint32_t nUniqueID = 0;
	uint32_t nAvatarID = 0;
	uint32_t nHealth = 100;
	float posX[2];
	float posY[2];// = 0;
	float posZ[2];// = 0;
	float orX[2];
	float orY[2];
	float orZ[2];
	float orW[2];
	int dir[2];
	float paintX[2];
	float paintY[2];
	float paintZ[2];
	//button commands
};
