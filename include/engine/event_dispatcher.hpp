#pragma once

#include <functional>

#include "keyboard_event.hpp"
#include "mouse_event.hpp"

namespace eng
{

/**
 * Class for consuming events and passing them to a supplied callback.
 */
class event_dispatcher final
{
    public:

        /** Alias for keyboard callback function type */
        using keyboard_callback = std::function<void(const keyboard_event&)>;

        /** Alias for mouse callback function type */
        using mouse_callback = std::function<void(const mouse_event&)>;

        /**
         * Create a enw event_dispatcher.
         *
         * @param keyboard_callback
         *   The function to call with keyboard events.
         *
         * @param mouse_callback
         *   The function to call with mouse events.
         */
        event_dispatcher(
            const keyboard_callback &keybaord_callback,
            const mouse_callback &mouse_callback) noexcept;

        /** Default */
        ~event_dispatcher() = default;

        /** Disabled */
        event_dispatcher(const event_dispatcher&) = delete;
        event_dispatcher& operator=(const event_dispatcher&) = delete;
        event_dispatcher(event_dispatcher&&) = delete;
        event_dispatcher& operator=(event_dispatcher&&) = delete;

        /**
         * Passes a given keyboard event to the set callback.
         *
         * @param event
         *   The event to dispatch.
         */
        void dispatch(const keyboard_event &event) const noexcept;

        /**
         * Passes a given mouse event to the set callback.
         *
         * @param event
         *   The event to dispatch.
         */
        void dispatch(const mouse_event &event) const noexcept;

    private:

        /** Callback to pass keyboard events to */
        keyboard_callback keyboard_callback_;

        /** Callback to pass mouse events to */
        mouse_callback mouse_callback_;
};

}
