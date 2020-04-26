#include "platform/event.h"

#include "platform/event_type.h"
#include "platform/keyboard_event.h"
#include "platform/mouse_event.h"

namespace eng
{

Event::Event(const KeyboardEvent event)
    : type_(EventType::KEYBOARD),
      event_(event)
{ }

Event::Event(const MouseEvent event)
    : type_(EventType::MOUSE),
      event_(event)
{ }

EventType Event::type() const
{
    return type_;
}

bool Event::is_key() const
{
    return std::holds_alternative<KeyboardEvent>(event_);
}

bool Event::is_key(Key key) const
{
    auto match = false;

    if(auto val = std::get_if<KeyboardEvent>(&event_) ; val)
    {
        match = val->key == key;
    }

    return match;
}

bool Event::is_key(Key key, KeyState state) const
{
    auto match = false;

    if(auto val = std::get_if<KeyboardEvent>(&event_) ; val)
    {
        match = (val->key) == key && (val->state == state);
    }

    return match;
}

KeyboardEvent Event::key() const
{
    if(!is_key())
    {
        throw Exception("not keyboard event");
    }

    return std::get<KeyboardEvent>(event_);
}

bool Event::is_mouse() const
{
    return std::holds_alternative<MouseEvent>(event_);
}

MouseEvent Event::mouse() const
{
    if(!is_mouse())
    {
        throw Exception("not mouse event");
    }

    return std::get<MouseEvent>(event_);
}

}
