#pragma once
namespace AutoFunc {
	void Init();

	void AutoJump(struct usercmd_s* cmd);
	void DuckTap(struct usercmd_s* cmd);
	void ConcurrentRun(struct usercmd_s* cmd);
}