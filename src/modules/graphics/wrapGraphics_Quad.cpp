#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Quad.h>

using love::graphics::opengl::Quad;

DECLARE_CLASS(Quad, love::GRAPHICS_QUAD_T);

WRAP_CLASS(Quad) {
    // flip
    // setViewport
    // getViewport

    return self;
}
