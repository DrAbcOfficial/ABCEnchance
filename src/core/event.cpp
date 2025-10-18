#include "event.h"

void EventManager::AddListener(std::type_index eventType, std::shared_ptr<EventListener> listener){
    listeners_[eventType].push_back(std::move(listener));
}

void EventManager::RemoveListener(std::type_index eventType, const EventListener* listener)
{
    auto it = listeners_.find(eventType);
    if (it != listeners_.end()) {
        auto& listenerList = it->second;
        listenerList.erase(
            std::remove_if(listenerList.begin(), listenerList.end(),
                [listener](const std::shared_ptr<EventListener>& ptr) {
                    return ptr.get() == listener;
                }),
            listenerList.end()
        );
        if (listenerList.empty()) {
            listeners_.erase(it);
        }
    }
}

void EventManager::TriggerEvent(const Event& event){
    std::type_index type = event.GetType();
    auto listenerIt = listeners_.find(type);
    if (listenerIt != listeners_.end()) {
        auto listenersCopy = listenerIt->second;
        for (const auto& listener : listenersCopy) {
            if (listener) {
                listener->OnEvent(&event);
            }
        }
    }
}

EventManager g_EventManager;