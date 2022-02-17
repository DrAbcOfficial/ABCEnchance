#pragma once
#ifdef  _DEBUG
class CHudCCTV {
public:
	void GLInit();
	int Init();
	int Draw(float flTime);
	void Reset();
private:
	GLint CCTVDownTga;
	GLint CCTVLogoTga;
	GLint CCTVTileTga;

	size_t DownLogoStartX = 0;
	size_t DownLogoStartY = 0;
	size_t DownLogoSize = 0;
	size_t LogoStartX = 0;
	size_t LogoStartY = 0;
	size_t LogoWidth = 0;
	size_t LogoHeight = 0;
	size_t TileStartX = 0;
	size_t TileStartY = 0;
	size_t TileWidth = 0;
	size_t TileHeight = 0;

};
extern CHudCCTV m_HudCCTV;
#endif