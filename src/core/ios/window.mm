#include "core/window.h"

#include <memory>

#import <UIKit/UIKit.h>

#import "core/ios/MetalViewController.h"
#include "graphics/render_system.h"
#include "graphics/render_target.h"
#include "log/log.h"

namespace iris
{

struct Window::implementation
{
};

Window::Window(std::uint32_t width, std::uint32_t height)
    : width_(width),
      height_(height),
      render_system_(nullptr),
      screen_target_(nullptr),
      impl_(nullptr)
{
    const auto bounds = [[UIScreen mainScreen] bounds];
    width_ = bounds.size.width;
    height_ = bounds.size.height;

    // we can now create a render system
    screen_target_ = std::make_unique<RenderTarget>(
        static_cast<std::uint32_t>(width_), static_cast<std::uint32_t>(height_));
    render_system_ = std::make_unique<RenderSystem>(width_, height_);
}

Window::~Window() = default;

std::optional<Event> Window::pump_event()
{
    const CFTimeInterval seconds = 0.000002;

    // run the default loop, this pumps touch events which will then be picked
    // up by our view
    auto result = kCFRunLoopRunHandledSource;
    do
    {
        result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, TRUE);
    } while(result == kCFRunLoopRunHandledSource);
    
    const auto *window = [[[UIApplication sharedApplication] windows] objectAtIndex:0];
    const auto *root_view_controller = static_cast<MetalViewController*>([window rootViewController]);
    
    std::optional<Event> event;
    
    // get next event from view (if one is available)
    if(!root_view_controller->events_.empty())
    {
        event = root_view_controller->events_.front();
        root_view_controller->events_.pop();
    }
    
    return event;
}

void Window::render(const Pipeline &pipeline) const
{
    render_system_->render(pipeline);
}

std::uint32_t Window::width() const
{
    return width_;
}

std::uint32_t Window::height() const
{
    return height_;
}

std::uint32_t Window::screen_scale()
{
    static std::uint32_t scale = 0u;

    if(scale == 0u)
    {
        scale = static_cast<std::uint32_t>([[UIScreen mainScreen] scale]);
    }

    return scale;
}

}