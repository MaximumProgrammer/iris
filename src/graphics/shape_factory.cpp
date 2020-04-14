#include "shape_factory.hpp"

#include <vector>

#include "mesh.hpp"
#include "texture.hpp"
#include "vector3.hpp"
#include "vertex_data.hpp"

namespace eng::shape_factory
{

mesh make_sprite(
    const vector3 &colour,
    texture &&tex)
{
    std::vector<vertex_data> vertices {
        { { -0.5f,  0.5f, 0.0f }, { } , colour, { 0.0f, 1.0f, 0.0f } },
        { {  0.5f,  0.5f, 0.0f }, { } , colour, { 1.0f, 1.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { } , colour, { 1.0f, 0.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f }, { } , colour, { 0.0f, 0.0f, 0.0f } }
    };

    std::vector<std::uint32_t> indices {
        0, 2, 1, 3, 2, 0
    };

    return {
        vertices,
        indices,
        std::move(tex)
    };
}

}

