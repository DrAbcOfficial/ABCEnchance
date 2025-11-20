#include "eventpp/callbacklist.h"
#define DEFINE_HUD_EVENT(name, returntype, ...) eventpp::CallbackList<returntype(__VA_ARGS__)> g_EventHud##name

extern DEFINE_HUD_EVENT(UpdateClientData, void, struct client_data_s*, float);
extern DEFINE_HUD_EVENT(TxferPredictionData, void, struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
extern DEFINE_HUD_EVENT(MousePressed, void, int);

namespace HudEvent {
	extern void OnUpdateClientData(client_data_s* cdata, float time);
	extern void OnTxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
	extern void OnIN_MouseEvent(int mstate);
}