#pragma once

#ifndef __SPRITES_RESOURCE_H__
#define __SPRITES_RESOURCE_H__

#include <vector>
#include <optional>

#pragma region hlsdk
#include "cdll_int.h"
#include "wrect.h"
#pragma endregion

class SpriteResource {
public:
	void VidInit();
	int GetSprite(size_t index);
	wrect_t* GetSpriteRect(size_t index);
	std::optional<int> GetSpriteIndex(const char* szSprite);
private:
	std::vector<client_sprite_t*> m_arySprites; // the sprites loaded from hud.txt
};
extern SpriteResource gSpriteRes;

#endif