#pragma once

#include <atomic>
#include <future>
#include <vector>
#include <any>

class CTaskItem {
public:
	CTaskItem(std::future<std::any>* futrue);
	void ContinueWith(std::function<void(std::any&)> func);
	bool Ready();
	bool Vaild();
	bool Excute();
private:
	std::atomic<std::future<std::any>*> m_pFutrue;
	std::function<void(std::any&)> m_pContinue;
};

class CTaskManager {
public:
	void CheckAll();
	CTaskItem* Start(std::future<std::any>* func);
private:
	std::vector<CTaskItem*> m_aryList;
};
CTaskManager* GetTaskManager();