#ifndef VGUI_IVanilliaPanel_H_
#define VGUI_IVanilliaPanel_H_

class IVanilliaPanel {
public:
	virtual void* GetVPanel() = 0;
	virtual void Think() = 0;
	virtual void PerformApplySchemeSettings() = 0;
	virtual void PaintTraverse() = 0;
	virtual void Repaint() = 0;
	virtual int IsWithinTraverse(int param_1, int param_2, bool param_3) = 0;
	virtual void GetInset(int* param_1, int* param_2, int* param_3, int* param_4) = 0;
	virtual void GetClipRect(int* param_1, int* param_2, int* param_3, int* param_4) = 0;
	virtual void OnChildAdded(uint param_1) = 0;
	virtual void OnSizeChanged(int param_1, int param_2) = 0;
	virtual void InternalFocusChanged(bool param_1) = 0;
	virtual void RequestInfo(void* pKeyValues) = 0;
	virtual void RequestFocus(int param_1) = 0;
	virtual void RequestFocusPrev(uint param_1) = 0;
	virtual void RequestFocusNext(uint param_1) = 0;
	virtual void OnMessage(void* pKeyValues, uint param_2) = 0;
	virtual int GetCurrentKeyFocus() = 0;
	virtual void* GetTabPosition() = 0;
	virtual char* GetName() = 0;
	virtual char* GetClassName() = 0;
	virtual void GetScheme() = 0;
	virtual void* IsProportional() = 0;
	virtual void* IsAutoDeleteSet() = 0;
	virtual void OnDelete() = 0;
	virtual void QueryInterface(int iEInterfaceID) = 0;
	virtual void* GetPanel() = 0;
	virtual void GetModuleName() = 0;
	virtual int GetMessageMap() = 0;
	virtual void* dtor() = 0;
	virtual void SetVisible(bool state) = 0;
	virtual bool IsVisible() = 0;
};
#endif