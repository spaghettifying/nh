#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

/**
 * Event system for plugin-wide communication
 * Allows modules to communicate without tight coupling
 */
class EventSystem {
public:
    using EventHandler = std::function<void(const std::string&, void*)>;
    using EventId = std::string;
    
    static EventSystem& GetInstance();
    
    // Subscribe to events
    void Subscribe(const EventId& eventId, EventHandler handler);
    
    // Publish events
    void Publish(const EventId& eventId, void* data = nullptr);
    
    // Unsubscribe from events
    void Unsubscribe(const EventId& eventId);
    
    // Common event types
    struct Events {
        static const EventId CONFIG_CHANGED;
        static const EventId PLAYER_CONNECTED;
        static const EventId PLAYER_DISCONNECTED;
        static const EventId MATCH_STARTED;
        static const EventId MATCH_ENDED;
        static const EventId GUI_TOGGLED;
    };
    
private:
    EventSystem() = default;
    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    
    std::unordered_map<EventId, std::vector<EventHandler>> handlers_;
};
