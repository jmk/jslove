#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Font.h>

#include <string>

using love::graphics::opengl::Font;

//DECLARE_CLASS(Font, love::GRAPHICS_FONT_T);

WRAP_CLASS(Font) {
    //getHeight
    //getWidth
    //getWrap
    //setLineHeight
    //getLineHeight

    return self;
}
