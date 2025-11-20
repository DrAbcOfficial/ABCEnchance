#include "MouseCode.h"
#include "hudevents.h"

DEFINE_HUD_EVENT(UpdateClientData, void, struct client_data_s*, float);
DEFINE_HUD_EVENT(TxferPredictionData, void, struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
DEFINE_HUD_EVENT(MousePressed, void, int);
namespace HudEvent {
	void OnUpdateClientData(struct client_data_s* cdata, float time) {
		g_EventHudUpdateClientData(cdata, time);
	}
	void OnTxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd) {
		g_EventHudTxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);
	}
	void OnIN_MouseEvent(int mstate) {
		auto MouseTest = [&](int mstate, int testBit, vgui::MouseCode enumMouse) {
			static int s_mousebits = 0;
			static int s_lastclick = 5;
			//������
			if ((mstate & testBit) != 0) {
				//֮ǰû��
				if ((s_mousebits & testBit) == 0) {
					//Press
					g_EventHudMousePressed(enumMouse);
					//g_pViewPort->OnMousePressed(enumMouse);
					//����Bit
					s_mousebits += testBit;
					if (s_lastclick == enumMouse) {
						//g_pViewPort->OnMouseDoublePressed(enumMouse);
						s_lastclick = vgui::MouseCode::MOUSE_LAST;
					}
					else
						s_lastclick = enumMouse;
				}
			}
			//����û��֮ǰ��
			else if ((s_mousebits & testBit) != 0) {
				//����Release
				s_mousebits -= testBit;
				//g_pViewPort->OnMouseReleased(enumMouse);
			}
			};
		/**
		* ������ 0
		* �Ҽ���� 1
		* �м���� 2
		* 4�����  3
		* 5�����  4
		**/
		for (size_t i = 0; i < 5; i++) {
			MouseTest(mstate, 1 << i, static_cast<vgui::MouseCode>(i));
		}
	}
}