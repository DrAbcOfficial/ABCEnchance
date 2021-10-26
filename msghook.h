#pragma once

pfnUserMsgHook MSG_HookUserMsg(char* szMsgName, pfnUserMsgHook pfn);
pfnUserMsgHook MSG_UnHookUserMsg(char* szMsgName);

#define HOOK_MESSAGE(x) MSG_HookUserMsg((char*)#x, __MsgFunc_##x);