#include "graphics/render_target.h"

#include <any>
#include <cstdint>
#include <memory>
#include <vector>

#import <Metal/Metal.h>

#include "core/data_buffer.h"
#include "core/exception.h"
#include "core/window.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris
{

struct RenderTarget::implementation
{
};

RenderTarget::RenderTarget(
    std::uint32_t width,
    std::uint32_t height)
    : colour_texture_(nullptr)
    , depth_texture_(nullptr)
    , impl_(std::make_unique<implementation>())
{
    const auto scale = Window::screen_scale();

    DataBuffer image_data(width * height * scale * scale * 4u);

    // create textures for colour and depth

    colour_texture_ = std::make_unique<Texture>(image_data, width * scale, height * scale, PixelFormat::RGBA);
    colour_texture_->set_flip(true);

    depth_texture_ = std::make_unique<Texture>(image_data, width * scale, height * scale, PixelFormat::DEPTH);
    depth_texture_->set_flip(true);
}

RenderTarget::~RenderTarget() = default;

std::any RenderTarget::native_handle() const
{
    return {};
}

Texture *RenderTarget::colour_texture() const
{
    return colour_texture_.get();
}

Texture *RenderTarget::depth_texture() const
{
    return depth_texture_.get();
}
}
