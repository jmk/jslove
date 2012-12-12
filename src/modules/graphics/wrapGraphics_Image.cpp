#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Image.h>

using love::graphics::opengl::Image;

DECLARE_CLASS(Image, love::GRAPHICS_IMAGE_T);

WRAP_FUNCTION(getWidth)
{
    Image* image = JSLExtractObject<Image>(ctx, self);
    float width = (image) ? image->getWidth() : 0;
    return JSValueMakeNumber(ctx, width);
}

WRAP_FUNCTION(getHeight)
{
    Image* image = JSLExtractObject<Image>(ctx, self);
    float height = (image) ? image->getHeight() : 0;
    return JSValueMakeNumber(ctx, height);
}

WRAP_CLASS(Image) {
    JSLAddFunction(ctx, self, "getWidth", getWidth);
    JSLAddFunction(ctx, self, "getHeight", getHeight);
    //setFilter
    //getFilter
    //setWrap
    //getWrap

    return self;
}
