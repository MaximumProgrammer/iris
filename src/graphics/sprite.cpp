#include "sprite.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "exception.hpp"
#include "log.hpp"
#include "material.hpp"
#include "material_factory.hpp"
#include "matrix4.hpp"
#include "quaternion.hpp"
#include "shape_factory.hpp"
#include "vector3.hpp"

namespace eng
{

sprite::sprite(
    const float x,
    const float y,
    const float width,
    const float height,
    const vector3 &colour)
    : mesh_(shape_factory::make_sprite(colour, texture::blank())),
      position_(x, y, 0.0f),
      orientation_(),
      scale_(width, height, 1.0f),
      model_(),
      material_(material_factory::basic_sprite()),
      wireframe_(false)
{
    LOG_ENGINE_INFO("entity", "constructed at: {}", position_);
}

void sprite::set_position(const vector3 &position)
{
    position_ = position;

    model_ = matrix4::make_translate(position_) * matrix4(orientation_) * matrix4::make_scale(scale_);
}

void sprite::set_orientation(const quaternion &orientation)
{
    orientation_ = orientation;
    model_ = matrix4::make_translate(position_) * matrix4(orientation_) * matrix4::make_scale(scale_);
}

matrix4 sprite::transform() const
{
    return model_;
}

const mesh& sprite::render_mesh() const
{
    return mesh_;
}

const material& sprite::mat() const
{
    return *material_;
}

bool sprite::should_render_wireframe() const
{
    return wireframe_;
}

void sprite::set_wireframe(const bool wireframe)
{
    wireframe_ = wireframe;
}

}

