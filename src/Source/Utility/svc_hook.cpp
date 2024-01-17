#include <metahook.h>
#include "plugins.h"
#include "svc_hook.h"
#include <exportfuncs.h>

extern void SysError(const char* message ...);

int SVC_LASTMSG = 50;

typedef struct svc_func_s
{
	unsigned char opcode;
	char* pszname;
	pfnSVCMsgHook pfnParse;
}
svc_func_t;

static svc_func_t* cl_parsefuncs = nullptr;
static byte* net_message_data = nullptr;
static int* net_message_cursize = nullptr;
static int* msg_readcount = nullptr;

void SVC_FillAddress(void)
{
	DWORD addr_svc_bad = (DWORD)g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, "svc_bad", sizeof("svc_bad") - 1);
	DWORD addr_svc_nop = (DWORD)g_pMetaHookAPI->SearchPattern((void*)addr_svc_bad, 0x100, "svc_nop", sizeof("svc_nop") - 1);

	int svc[6] = { 0, addr_svc_bad, 0, 1, addr_svc_nop, 0 };
	char* sig_svc = (char*)svc;

	DWORD addr_cl_parsefuncs = (DWORD)g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, sig_svc, sizeof(svc));

	if (addr_cl_parsefuncs)
	{
		cl_parsefuncs = (svc_func_t*)addr_cl_parsefuncs;

		while (SVC_LASTMSG)
		{
			if (cl_parsefuncs[SVC_LASTMSG].opcode == 0xFF)
			{
				SVC_LASTMSG -= 1;
				break;
			}

			SVC_LASTMSG++;
		}
	}
	DWORD addr;
#define R_MSGREADCOUNT_SIG "\x0F\x8B\xAE\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\x48"
	{
		addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_MSGREADCOUNT_SIG, Sig_Length(R_MSGREADCOUNT_SIG));
		auto a = addr - (DWORD)g_dwEngineBase;
		Sig_AddrNotFound(msg_readcount);
		msg_readcount = (int*)(*(DWORD*)(addr+7));
	}
#define R_NETMESSAGE_MAX_SIG "\x33\xC0\xC7\x05\x2A\x2A\x2A\x2A\x00\x00\x04\x00"
	{
		addr = (DWORD)g_pMetaHookAPI->SearchPattern(g_dwEngineBase, g_dwEngineSize, R_NETMESSAGE_MAX_SIG, Sig_Length(R_NETMESSAGE_MAX_SIG));
		Sig_AddrNotFound(netmessage_max);
		net_message_data = (byte*)(*(DWORD*)(addr + 4) - 4);
		net_message_cursize = (int*)(*(DWORD*)(addr + 4) + 4);
	}
}

pfnSVCMsgHook SVC_HookFunc(int opcode, pfnSVCMsgHook pfnParse)
{
	pfnSVCMsgHook pfnResult = cl_parsefuncs[opcode].pfnParse;

	cl_parsefuncs[opcode].pfnParse = pfnParse;
	return pfnResult;
}

void* SVC_GetBuffer(void)
{
	return net_message_data + *msg_readcount;
}

int SVC_GetBufferSize(void)
{
	return *net_message_cursize - *msg_readcount;
}