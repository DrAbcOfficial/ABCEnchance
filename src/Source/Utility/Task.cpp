#include "Task.h"

static CTaskManager s_TaskManager;

CTaskManager* GetTaskManager() {
	return &s_TaskManager;
}
bool CTaskManager::Has(ITaskItem* check){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
		if ((*iter) == check)
			return true;
	}
	return false;
}
void CTaskManager::Shutdown() {
	for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
		auto item = *iter;
		delete item;
	}
	m_aryList.clear();

	for (auto iter = m_aryPending.begin(); iter != m_aryPending.end(); iter++) {
		auto item = *iter;
		delete item;
	}
	m_aryPending.clear();
}
void CTaskManager::CheckAll(){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
		auto item = *iter;
		if (item->IsReady()) {
			//转移所有权，防止栈被干烂
			auto caller = std::move(item->GetCaller());
			delete item;
			iter = m_aryList.erase(iter);
			std::invoke(caller);
		}
		else
			iter++;
	}
	//add pending
	if (m_aryPending.size() > 0)
		m_aryList.splice(m_aryList.end(), m_aryPending);
}
