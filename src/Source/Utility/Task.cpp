#include "Task.h"

static CTaskManager s_TaskManager;

CTaskManager* GetTaskManager() {
	return &s_TaskManager;
}

CTaskItem::CTaskItem(std::future<std::any>* futrue){
	m_pFutrue = futrue;
}

void CTaskItem::ContinueWith(std::function<void(std::any&)> func){
	this->m_pContinue = func;
}

bool CTaskItem::Ready(){
	return m_pFutrue.load()->wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool CTaskItem::Vaild(){
	return m_pFutrue.load()->valid();
}

bool CTaskItem::Excute(){
	if (m_pContinue != nullptr) {
		m_pContinue(m_pFutrue.load()->get());
		return true;
	}
	return false;
}

void CTaskManager::CheckAll(){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
		auto obj = *iter;
		if (obj->Vaild()) {
			if (obj->Ready()) {
				obj->Excute();
				delete obj;
				iter = m_aryList.erase(iter);
				continue;
			}
			iter++;
		}
		else {
			delete obj;
			iter = m_aryList.erase(iter);
			continue;
		}
		iter++;
	}
}

CTaskItem* CTaskManager::Start(std::future<std::any>* func){
	CTaskItem* item = new CTaskItem(func);
	m_aryList.push_back(item);
	return item;
}
