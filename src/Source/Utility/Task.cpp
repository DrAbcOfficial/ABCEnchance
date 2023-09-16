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

void CTaskItem::Excute(){
	if (m_pContinue != nullptr)
		m_pContinue(m_pValue);
}

void CTaskItem::Start(){
	m_pThread.detach();
}


CTaskItem* CTaskManager::Add(std::future<std::any>& func){
	CTaskItem* item = new CTaskItem(func);
	m_aryList.push_back(item);
	return item;
}

void CTaskManager::CheckAll(){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
		if ((*iter)->IsReady()) {
			(*iter)->Excute();
			iter = m_aryList.erase(iter);
		}
		else
			iter++;
	}
}
