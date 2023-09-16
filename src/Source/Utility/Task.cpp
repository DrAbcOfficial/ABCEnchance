#include "Task.h"

static CTaskManager s_TaskManager;

CTaskManager* GetTaskManager() {
	return &s_TaskManager;
}

CTaskItem::CTaskItem(std::future<std::any>& futrue){
	auto& refB = m_bReady;
	auto& refV = m_pValue;
	m_pThread = std::thread(
		[&refB, &refV, futrue = std::move(futrue)]() mutable{
			refV = futrue.get();
			refB = true;
		}
	);
}

CTaskItem::~CTaskItem(){
	if (m_pThread.joinable())
		m_pThread.join();
}

CTaskItem* CTaskItem::ContinueWith(std::function<void(std::any&)> func){
	m_pContinue = std::move(func);
	return this;
}

bool CTaskItem::IsReady(){
	return m_bReady;
}

std::function<void(std::any&)> CTaskItem::GetContinue(){
	return m_pContinue;
}

std::any CTaskItem::GetValue(){
	return m_pValue;
}

CTaskItem* CTaskItem::Start(){
	m_pThread.detach();
	return this;
}

CTaskItem* CTaskManager::Add(std::future<std::any>& func){
	CTaskItem* item = new CTaskItem(func);
	m_aryList.push_back(item);
	return item;
}

bool CTaskManager::Has(CTaskItem* check){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
		if ((*iter) == check)
			return true;
	}
	return false;
}

void CTaskManager::CheckAll(){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
		auto item = *iter;
		if (item->IsReady()) {
			//转移所有权，防止栈被干烂
			auto callback = std::move(item->GetContinue());
			auto val = std::move(item->GetValue());
			delete item;
			iter = m_aryList.erase(iter);
			std::invoke(callback, val);
		}
		else
			iter++;
	}
}
