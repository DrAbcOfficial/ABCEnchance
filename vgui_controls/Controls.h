//===== Copyright ?1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef CONTROLS_H
#define CONTROLS_H

#ifdef _WIN32
#pragma once
#endif

#include <interface.h>
#include <vstdlib/IKeyValuesSystem.h>
#include <tier0/dbg.h>
#include <tier1/strtools.h>

#include <interface/ISystemABC.h>

#include <vgui/VGUI.h>
#include <vgui/IInput2.h>
#include <vgui/IInputInternal.h>
#include <vgui/IScheme2.h>
#include <vgui/ISurface2.h>
#include <vgui/IVGUI.h>
#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui/MouseCode.h>
#include <vgui/KeyCode.h>

extern IFileSystem *g_pFullFileSystem;

extern vgui::IInput2 *g_pVGuiInput2;
extern vgui::ISchemeManager2 * g_pVGuiSchemeManager2;
extern vgui::ISurface2 *g_pVGuiSurface;
extern vgui::ISystemABC* g_pVGuiSystemABC;
extern vgui::IVGui *g_pVGui;
extern vgui::IPanel *g_pVGuiPanel;
extern vgui::ILocalize *g_pVGuiLocalize;

namespace vgui
{

// handles the initialization of the vgui interfaces
// interfaces (listed below) are first attempted to be loaded from primaryProvider, then secondaryProvider
// moduleName should be the name of the module that this instance of the vgui_controls has been compiled into
bool VGui_InitInterfacesList( const char *moduleName, CreateInterfaceFn *factoryList, int numFactories );

// returns the name of the module as specified above
const char *GetControlsModuleName();

class IPanel;
class IInput2;
class ISchemeManager;
class ISurface;
class ISystemABC;
class IVGui;

//-----------------------------------------------------------------------------
// Backward compat interfaces, use the interfaces grabbed in tier3
// set of accessor functions to vgui interfaces
// the appropriate header file for each is listed above the item
//-----------------------------------------------------------------------------

// #include <vgui/IInput.h>
inline vgui::IInput2 *input()
{
	return g_pVGuiInput2;
}

inline vgui::IInputInternal *inputinternal()
{
	return (vgui::IInputInternal *)g_pVGuiInput2;
}

// #include <vgui/IScheme2.h>
inline vgui::ISchemeManager2 *scheme()
{
	return g_pVGuiSchemeManager2;
}

// #include <vgui/ISurface2.h>
inline vgui::ISurface2 *surface()
{
	return g_pVGuiSurface;
}

// #include <vgui/ISystem.h>
inline vgui::ISystemABC *system()
{
	return g_pVGuiSystemABC;
}

// #include <vgui/IVGui.h>
inline vgui::IVGui *ivgui()
{
	return g_pVGui;
}

// #include <vgui/IPanel.h>
inline vgui::IPanel *ipanel()
{
	return g_pVGuiPanel;
}

inline vgui::ILocalize *localize()
{
	return g_pVGuiLocalize;
}
inline IFileSystem* filesystem() {
	return g_pFullFileSystem;
}

// predeclare all the vgui control class names
class AnalogBar;
class AnimatingImagePanel;
class AnimationController;
class BuildModeDialog;
class Button;
class CheckButton;
class CheckButtonList;
class ComboBox;
class DirectorySelectDialog;
class Divider;
class EditablePanel;
class FileOpenDialog;
class Frame;
class GraphPanel;
class HTML;
class ImagePanel;
class Label;
class ListPanel;
class ListViewPanel;
class Menu;
class MenuBar;
class MenuButton;
class MenuItem;
class MessageBox;
class Panel;
class PanelListPanel;
class ProgressBar;
class ProgressBox;
class PropertyDialog;
class PropertyPage;
class PropertySheet;
class QueryBox;
class RadioButton;
class RichText;
class ScrollBar;
class ScrollBarSlider;
class SectionedListPanel;
class Slider;
class Splitter;
class TextEntry;
class ToggleButton;
class Tooltip;
class TreeView;
class CTreeViewListControl;
class URLLabel;
class WizardPanel;
class WizardSubPanel;

// vgui controls helper classes
class BuildGroup;
class FocusNavGroup;
class IBorder;
class IImage;
class Image;
class ImageList;
class TextImage;

} // namespace vgui

// hotkeys disabled until we work out exactly how we want to do them
#define VGUI_HOTKEYS_ENABLED
// #define VGUI_DRAW_HOTKEYS_ENABLED

#define USING_BUILD_FACTORY( className )				\
	extern className *g_##className##LinkerHack;		\
	className *g_##className##PullInModule = g_##className##LinkerHack;

#define USING_BUILD_FACTORY_ALIAS( className, factoryName )				\
	extern className *g_##factoryName##LinkerHack;		\
	className *g_##factoryName##PullInModule = g_##factoryName##LinkerHack;

#endif // CONTROLS_H
