#include "render_system.hpp"

#include <string>
#include <utility>

#include "auto_bind.hpp"
#include "entity.hpp"
#include "gl/buffer.hpp"
#include "gl/opengl.hpp"
#include "gl/vertex_state.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "vector3.hpp"

namespace
{
    // hard coded shaders

    static const std::string vertex_source { R"(
    #version 330 core
    precision mediump float;
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 normal;
    layout (location = 2) in vec3 colour;
    layout (location = 3) in vec3 tex;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    out vec3 fragPos;
    out vec2 texCoord;
    out vec3 col;
    out vec3 norm;
    void main()
    {
        col = colour;
        norm = mat3(transpose(inverse(transpose(model)))) * normal;
        texCoord = vec2(tex.x, tex.y);
        fragPos = vec3(transpose(model) * vec4(position, 1.0));
        gl_Position = transpose(projection) * transpose(view) * vec4(fragPos, 1.0);
    }
)" };

    static const std::string fragment_source { R"(
    #version 330 core
    precision mediump float;
    in vec2 texCoord;
    in vec3 col;
    in vec3 norm;
    in vec3 fragPos;
    out vec4 outColor;
    uniform sampler2D texture1;
    uniform vec3 light;
    void main()
    {
        const vec3 amb = vec3(0.2, 0.2, 0.2);
        const vec3 lightColour = vec3(1.0, 1.0, 1.0);
        vec3 n = normalize(norm);
        vec3 light_dir = normalize(light - fragPos);
        float diff = max(dot(n, light_dir), 0.0);
        vec3 diffuse = diff * lightColour;
        vec3 l = amb + diffuse;
        outColor = vec4(l, 1.0) * texture(texture1, texCoord) * vec4(col, 1.0);
    }
)"};

}

namespace eng
{

render_system::render_system(
            std::shared_ptr<camera> c,
            std::shared_ptr<window> w,
            const float width,
            const float height)
    : scene_(),
      camera_(c),
      window_(w),
      material_(vertex_source, fragment_source),
      light_position()
{
    // opengl setup

    ::glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl::check_opengl_error("could not set clear colour");

    ::glEnable(GL_DEPTH_TEST);
    gl::check_opengl_error("could not enable depth testing");

    ::glDepthFunc(GL_LESS);
    gl::check_opengl_error("could not set depth test function");

    ::glViewport(
        0,
        0,
        static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    gl::check_opengl_error("could not set viewport");
}

void render_system::add(std::shared_ptr<entity> e)
{
    scene_.emplace_back(e);
}

void render_system::render() const
{
    window_->pre_render();

    // render each element in scene
    for(const auto &e : scene_)
    {
        // bind material so render with it
        auto_bind<material> auto_program{ material_ };

        const auto program = material_.native_handle<std::uint32_t>();

        // set uniforms

        const auto proj_uniform = ::glGetUniformLocation(program, "projection");
        gl::check_opengl_error("could not get projection uniform location");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera_->projection().data());
        gl::check_opengl_error("could not set projection matrix uniform data");

        const auto view_uniform = ::glGetUniformLocation(program, "view");
        gl::check_opengl_error("could not get view uniform location");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera_->view().data());
        gl::check_opengl_error("could not set view matrix uniform data");

        const auto light_uniform = ::glGetUniformLocation(program, "light");
        gl::check_opengl_error("could not get light uniform location");

        ::glUniform3f(light_uniform, light_position.x, light_position.y, light_position.z);
        gl::check_opengl_error("could not set light uniform data");

        const auto model_uniform = ::glGetUniformLocation(program, "model");
        gl::check_opengl_error("could not get model uniform location");

        // render each mesh in element
        for(const auto &m : e->meshes())
        {
            // bind mesh so the final draw call renders it
            auto_bind<mesh> auto_mesh{ m };

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, m.model().data());
            gl::check_opengl_error("could not set model matrix uniform data");

            // draw!
            ::glDrawElements(GL_TRIANGLES, m.indices().size(), GL_UNSIGNED_INT, 0);
            gl::check_opengl_error("could not draw triangles");
        }
    }

    window_->post_render();
}

void render_system::set_light_position(const vector3 &position) noexcept
{
    light_position = position;
}

void render_system::set_wireframe_mode(bool wireframe)
{
    const auto mode = wireframe
        ? GL_LINE
        : GL_FILL;
    ::glPolygonMode(GL_FRONT_AND_BACK, mode);
}

}

