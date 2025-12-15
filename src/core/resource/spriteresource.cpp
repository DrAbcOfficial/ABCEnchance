#include <metahook.h>

#include "utility/util.h"

#include "spriteresource.h"
#include <Controls.h>

SpriteResource gSpriteRes;

void SpriteResource::VidInit() {
	int screen_w, screen_h;
	vgui::surface()->GetScreenSize(screen_w, screen_h);

	int iResSize = screen_w < 640 ? 320 : 640;
	if (m_arySprites.size() == 0) {
		int size;
		client_sprite_t* arySpritelist = SPR_GetList("sprites/hud.txt", &size);
		if (arySpritelist) {
			client_sprite_t* p = arySpritelist;
			for (int i = 0; i < size; i++) {
				if (p->iRes == iResSize) {
					char sz[MAX_PATH];
					std::sprintf(sz, "sprites/%s.spr", p->szSprite);
					client_sprite_t* item = new client_sprite_t();
					std::memcpy(item, p, sizeof(client_sprite_t));
					item->hspr = SPR_Load(sz);
					m_arySprites.push_back(item);
				}
				p++;
			}
		}
	}
	else {
		for (auto iter = m_arySprites.begin(); iter != m_arySprites.end(); iter++) {
			client_sprite_t* item = *iter;
			if (item->iRes == iResSize) {
				char sz[MAX_PATH];
				std::sprintf(sz, "sprites/%s.spr", item->szSprite);
				item->hspr = SPR_Load(sz);
			}
		}
	}
}

int SpriteResource::GetSprite(size_t index) {
	return (index < 0) ? 0 : m_arySprites[index]->hspr;
}

wrect_t* SpriteResource::GetSpriteRect(size_t index) {
	if (index >= 0 && index < m_arySprites.size())
		return &m_arySprites[index]->rc;
	return nullptr;
}

std::optional<int> SpriteResource::GetSpriteIndex(const char* szSprite) {
	for (size_t i = 0; i < m_arySprites.size(); i++) {
		if (!std::strncmp(szSprite, m_arySprites[i]->szName, 64))
			return i;
	}
	return std::nullopt;
}