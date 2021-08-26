#include <metahook.h>
#include "parsemsg.h"
#include "pm_shared.h"
#include "msghook.h"

#include "hud.h"
#include "deathmsg.h"

CHudDeathMsg m_HudDeathMsg;
pfnUserMsgHook m_pfnDeathMsg;

int __MsgFunc_DeathMsg(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	char killedwith[32];
	strcpy(killedwith, "d_");
	strncat(killedwith, READ_STRING(), 32);

	return m_pfnDeathMsg(pszName, iSize, pbuf);
}

int CHudDeathMsg::Draw(float flTime)
{
	return 1;
}
void CHudDeathMsg::Reset(void)
{

}

int CHudDeathMsg::Init(void)
{
	m_pfnDeathMsg = HOOK_MESSAGE(DeathMsg);
	return 1;
}