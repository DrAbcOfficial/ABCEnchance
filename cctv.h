#pragma once
class CHudCCTV {
public:
	int Init();
	int Draw(float flTime);
	void Reset();
private:
	GLint CCTVDownTga;
	GLint CCTVLogoTga;
	GLint CCTVTileTga;

	int DownLogoStartX;
	int DownLogoStartY;
	int DownLogoSize;
	int LogoStartX;
	int LogoStartY;
	int LogoWidth;
	int LogoHeight;
	int TileStartX;
	int TileStartY;
	int TileWidth;
	int TileHeight;
};
extern CHudCCTV m_HudCCTV;