#pragma once
#ifndef ISYSTEM_H
#include <Interface/VGUI/ISystem.h>
#endif


namespace vgui
{

	class ISystemABC : public ISystem{
	public:
		// Convert a windows virtual key code to a VGUI key code.
		virtual KeyCode KeyCode_VirtualKeyToVGUI(int keyCode) = 0;
		virtual int KeyCode_VGUIToVirtualKey(KeyCode keyCode) = 0;
		virtual const char* GetStartMenuFolderPath() = 0;
		virtual const char* GetAllUserDesktopFolderPath() = 0;
		virtual const char* GetAllUserStartMenuFolderPath() = 0;
	};
}
