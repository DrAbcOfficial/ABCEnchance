#pragma once

#include <future>
#include <vector>
#include <any>

class CTaskItem {
public:
	CTaskItem(std::future<std::any>& futrue);
	~CTaskItem();
	CTaskItem* ContinueWith(std::function<void(std::any&)> func);
	bool IsReady();
	void Excute();
	void Start();

private:
	std::thread m_pThread;
	std::atomic_bool m_bReady = false;
	std::any m_pValue;
	std::function<void(std::any&)> m_pContinue = nullptr;
};

class CTaskManager {
public:
	CTaskItem* Add(std::future<std::any>& func);
	void CheckAll();
private:
	std::vector<CTaskItem*> m_aryList;
};
CTaskManager* GetTaskManager();