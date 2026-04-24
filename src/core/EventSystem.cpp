#include "EventSystem.h"
#include <algorithm>
#include <iostream>

// Static event IDs
const EventSystem::EventId EventSystem::Events::CONFIG_CHANGED = "config_changed";
const EventSystem::EventId EventSystem::Events::PLAYER_CONNECTED = "player_connected";
const EventSystem::EventId EventSystem::Events::PLAYER_DISCONNECTED = "player_disconnected";
const EventSystem::EventId EventSystem::Events::MATCH_STARTED = "match_started";
const EventSystem::EventId EventSystem::Events::MATCH_ENDED = "match_ended";
const EventSystem::EventId EventSystem::Events::GUI_TOGGLED = "gui_toggled";

EventSystem& EventSystem::GetInstance() {
    static EventSystem instance;
    return instance;
}

void EventSystem::Subscribe(const EventId& eventId, EventHandler handler) {
    handlers_[eventId].push_back(handler);
}

void EventSystem::Publish(const EventId& eventId, void* data) {
    auto it = handlers_.find(eventId);
    if (it != handlers_.end()) {
        for (const auto& handler : it->second) {
            try {
                handler(eventId, data);
            } catch (const std::exception& e) {
                std::cerr << "Error in event handler for " << eventId << ": " << e.what() << std::endl;
            }
        }
    }
}

void EventSystem::Unsubscribe(const EventId& eventId) {
    handlers_.erase(eventId);
}
