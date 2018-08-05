#include "material.hpp"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "exception.hpp"
#include "gl/opengl.hpp"
#include "gl/shader.hpp"
#include "gl/shader_type.hpp"

namespace eng
{

/**
 * Struct containing implementation specific data.
 */
struct material::implementation final
{
    /** Simple constructor which takes a value for each member. */
    implementation(std::uint32_t program)
        : program(program)
    { }

    /** Default */
    implementation() = default;
    ~implementation() = default;
    implementation(const implementation&) = default;
    implementation& operator=(const implementation&) = default;
    implementation(implementation&&) = default;
    implementation& operator=(implementation&&) = default;

    /** Opengl handle for program. */
    std::uint32_t program;
};


material::material(
    const std::string &vertex_shader_source,
    const std::string &fragment_shader_source)
    : impl_(nullptr)
{
    const auto program = ::glCreateProgram();
    check_opengl_error("could not create new program");

    const shader vertex_shader{ vertex_shader_source, shader_type::VERTEX };
    const shader fragment_shader{ fragment_shader_source, shader_type::FRAGMENT };

    ::glAttachShader(program, vertex_shader.native_handle());
    check_opengl_error("could not attach vertex shader");

    ::glAttachShader(program, fragment_shader.native_handle());
    check_opengl_error("could not attach fragment shader");

    ::glLinkProgram(program);

    std::int32_t programparam = 0;

    ::glGetProgramiv(program, GL_LINK_STATUS, &programparam);

    // if program failed to link then get the opengl error
    if(programparam != GL_TRUE)
    {
        ::glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programparam);
        eng::check_opengl_error("could not get program log length");

        if(programparam == 0)
        {
            throw exception("program link failed: no log");
        }
        else
        {
            std::vector<char> error_log(programparam);

            // get opengl error log
            std::int32_t log_length = 0;
            ::glGetProgramInfoLog(
                program,
                static_cast<std::int32_t>(error_log.size()),
                &log_length,
                error_log.data());
            eng::check_opengl_error("failed to get error log");

            const std::string error(error_log.data(), log_length);
            throw exception("program link failed: " + error);
        }
    }

    impl_ = std::make_unique<implementation>(program);
}

material::~material()
{
    ::glDeleteProgram(impl_->program);
}

/** Default */
material::material(material &&other) = default;
material& material::operator=(material &&other) = default;

std::any material::native_handle() const
{
    return std::any { impl_->program };
}

}
