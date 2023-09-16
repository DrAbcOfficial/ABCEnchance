#pragma once

#include <future>
#include <vector>
#include <any>

class CTaskItem {
public:
	CTaskItem(std::future<std::any>& futrue);
	~CTaskItem();
	//continue with in main thread
	CTaskItem* ContinueWith(std::function<void(std::any&)> func);
	//start task
	CTaskItem* Start();
	//is ready
	bool IsReady();

	std::function<void(std::any&)> GetContinue();
	std::any GetValue();
private:
	std::thread m_pThread;
	std::atomic_bool m_bReady = false;
	std::any m_pValue;
	std::function<void(std::any&)> m_pContinue = nullptr;
};

class CTaskManager {
public:
	//add a new async task
	CTaskItem* Add(std::future<std::any>& func);
	//has this task?
	bool Has(CTaskItem* check);
	//check per frame
	void CheckAll();
private:
	std::vector<CTaskItem*> m_aryList;
};
CTaskManager* GetTaskManager();