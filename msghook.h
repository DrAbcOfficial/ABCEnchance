#pragma once

pfnUserMsgHook MSG_HookUserMsg(char* szMsgName, pfnUserMsgHook pfn);
pfnUserMsgHook MSG_UnHookUserMsg(char* szMsgName);

#define HOOK_MESSAGE(x) MSG_HookUserMsg(#x, __MsgFunc_##x);
#define HOOK_COMMAND(x, y) gEngfuncs.pfnAddCommand( x, __UserCmd_##y );