#pragma once

#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <concepts>
#include <algorithm>
#include <string>

class Event;

class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void OnEvent(const Event* event) = 0;
};

// 事件基类
class Event {
public:
    virtual ~Event() = default;
    virtual std::type_index GetType() const = 0;
};


template<typename E>
class ConcreteEvent : public Event {
public:
    virtual std::type_index GetType() const override {
        return typeid(E);
    }
};

// 事件管理器
class EventManager {
public:
    void AddListener(std::type_index eventType, std::shared_ptr<EventListener> listener);

    void RemoveListener(std::type_index eventType, const EventListener* listener);

    // 触发事件
    void TriggerEvent(const Event& event);

    // 模板版本的触发事件，要求 E 派生自 Event
    template<typename E>
        requires std::derived_from<E, Event>
    void TriggerEvent(const E& event) {
        TriggerEvent(static_cast<const Event&>(event));
    }

    // 禁止复制和移动
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
    EventManager(EventManager&&) = delete;
    EventManager& operator=(EventManager&&) = delete;
    EventManager() = default;

private:
    // 存储按名字的监听器：事件名 -> 监听器列表
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<EventListener>>> listeners_;
};

extern EventManager g_EventManager;
#endif // EVENT_SYSTEM_H