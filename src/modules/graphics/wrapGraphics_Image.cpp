#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Image.h>

#include <string>

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

WRAP_FUNCTION(setFilter)
{
    Image* image = JSLExtractObject<Image>(ctx, self);
    if (image and argCount == 2) {
        std::string min = JSLGetString(ctx, args[0]);
        std::string mag = JSLGetString(ctx, args[1]);

        // Set filter mode.
        Image::Filter f;

        if (not Image::getConstant(min.c_str(), f.min)) {
            printf("ERROR: Invalid min filter: %s\n", min.c_str());
            goto undefined;
        }

        if (not Image::getConstant(mag.c_str(), f.mag)) {
            printf("ERROR: Invalid mag filter: %s\n", mag.c_str());
            goto undefined;
        }

        image->setFilter(f);
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_CLASS(Image) {
    JSLAddFunction(ctx, self, "getWidth", getWidth);
    JSLAddFunction(ctx, self, "getHeight", getHeight);
    JSLAddFunction(ctx, self, "setFilter", setFilter);
    //getFilter
    //setWrap
    //getWrap

    return self;
}
