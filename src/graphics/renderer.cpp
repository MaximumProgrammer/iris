#include "graphics/renderer.h"

#include "core/exception.h"

namespace iris
{

void Renderer::render()
{
    pre_render();

    // call each command with the appropriate handler
    for (auto &command : render_queue_)
    {
        switch (command.type())
        {
            case RenderCommandType::UPLOAD_TEXTURE:
                execute_upload_texture(command);
                break;
            case RenderCommandType::PASS_START:
                execute_pass_start(command);
                break;
            case RenderCommandType::DRAW: execute_draw(command); break;
            case RenderCommandType::PASS_END: execute_pass_end(command); break;
            case RenderCommandType::PRESENT: execute_present(command); break;
            default: throw Exception("unknown render queue command");
        }
    }

    post_render();
}

void Renderer::pre_render()
{
    // default is to do nothing
}

void Renderer::execute_upload_texture(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_pass_start(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_draw(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_pass_end(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::execute_present(RenderCommand &)
{
    // default is to do nothing
}

void Renderer::post_render()
{
    // default is to do nothing
}

}