#include "graphics/render_graph/colour_node.h"

#include "core/colour.h"
#include "graphics/render_graph/compiler.h"

namespace iris
{

ColourNode::ColourNode(const Colour &colour)
    : colour_(colour)
{
}

void ColourNode::accept(Compiler &compiler) const
{
    compiler.visit(*this);
}

Colour ColourNode::colour() const
{
    return colour_;
}

}