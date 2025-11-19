#ifndef VGUI_SPR_IMAGE_H
#define VGUI_SPR_IMAGE_H

#include <string>
#include <vgui/IImage.h>
#include "PHandle.h"
#include <Color.h>

namespace vgui {
	class Panel;
	class CSPRImage : public vgui::IImage
	{
	public:
		CSPRImage();
		/**
		 * Creates an image from file.
		 * @see LoadImage
		 */
		CSPRImage(const char* pFilePath);

		/**
		 * Loads a .tga image from path pFilePath.
		 * .tga extension doesn't need to be specified.
		 */
		void LoadSprImage(const char* pFilePath);
		void SetTextureID(int id);

		/// <summary>
		/// Reset SPR id
		/// </summary>
		void Reset();
		/// <summary>
		/// Set Animate Rate
		/// </summary>
		/// <param name="flTime">framerate</param>
		void SetFramerate(float flTime);

		void SetFrame(float flFrame);

		void SetLoop(bool loop);

		void Animate();

		bool IsInTheEnd();

		void SetRect(int l, int r, int t, int b);
		/// <summary>
		/// Set Rendermode
		/// </summary>
		/// <param name="mode"></param>
		void SetRenderMode(int mode);

		virtual void Paint();
		virtual void SetPos(int x, int y);
		virtual void GetContentSize(int& wide, int& tall);
		virtual void GetSize(int& wide, int& tall);
		virtual void SetSize(int wide, int tall);
		virtual void SetColor(Color col);

	private:
		std::string m_szPath;
		Color m_Color = Color(255, 255, 255, 255);
		float m_flFrame = 0;
		float m_flAnimeTime = 0;
		float m_flFrameRate = 0;

		bool m_bSequenceLoop = false;

		int m_iTextureID = -1;
		int m_iAllFrames = 0;
		int m_nX = 0, m_nY = 0;
		int m_nLeft = 0, m_nRight = 0, m_nTop = 0, m_nBottom = 0;
		int m_iRenderMode = 0;
		int m_wide = 64, m_tall = 64;
	};
}
#endif
