#include "core/start.h"

#include <memory>

#include "core/root.h"
#include "graphics/macos/macos_window_manager.h"
#include "graphics/metal/metal_mesh_manager.h"
#include "graphics/metal/metal_texture_manager.h"
#include "graphics/opengl/opengl_mesh_manager.h"
#include "graphics/opengl/opengl_texture_manager.h"
#include "log/log.h"
#include "log/logger.h"

namespace
{

void register_apis()
{
    iris::Root::register_graphics_api(
        "metal",
        std::make_unique<iris::MacosWindowManager>(),
        std::make_unique<iris::MetalMeshManager>(),
        std::make_unique<iris::MetalTextureManager>());

    iris::Root::register_graphics_api(
        "opengl",
        std::make_unique<iris::MacosWindowManager>(),
        std::make_unique<iris::OpenGLMeshManager>(),
        std::make_unique<iris::OpenGLTextureManager>());

    iris::Root::set_graphics_api("metal");
}

}

namespace iris
{

void start(int argc, char **argv, std::function<void(int, char **)> entry)
{
    LOG_ENGINE_INFO("start", "engine start");

    register_apis();

    entry(argc, argv);
}

void start_debug(int argc, char **argv, std::function<void(int, char **)> entry)
{
    // enable engine logging
    Logger::instance().set_log_engine(true);

    LOG_ENGINE_INFO("start", "engine start (with debugging)");

    register_apis();

    entry(argc, argv);
}

}