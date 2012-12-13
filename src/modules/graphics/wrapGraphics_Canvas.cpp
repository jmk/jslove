#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Canvas.h>
#include <graphics/opengl/Image.h>
#include <graphics/Color.h>

using love::graphics::Color;
using love::graphics::opengl::Canvas;
using love::graphics::opengl::Image;

//DECLARE_CLASS(Canvas, love::GRAPHICS_CANVAS_T);

WRAP_FUNCTION(setFilter)
{
    Canvas* canvas = JSLExtractObject<Canvas>(ctx, self);
    if (canvas and argCount == 2) {
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

        canvas->setFilter(f);
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(clear)
{
    Canvas* canvas = JSLExtractObject<Canvas>(ctx, self);
    if (canvas) {
        // TODO overloads:
        //   - array of colors

        if (argCount == 4) {
            Color color;
            color.r = (unsigned char) JSLGetNumber(ctx, args[0]);
            color.g = (unsigned char) JSLGetNumber(ctx, args[1]);
            color.b = (unsigned char) JSLGetNumber(ctx, args[2]);
            color.a = (unsigned char) JSLGetNumber(ctx, args[3]);

            canvas->clear(color);
        }
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(getWidth)
{
    Canvas* canvas = JSLExtractObject<Canvas>(ctx, self);
    float width = (canvas) ? canvas->getWidth() : 0;
    return JSValueMakeNumber(ctx, width);
}

WRAP_FUNCTION(getHeight)
{
    Canvas* canvas = JSLExtractObject<Canvas>(ctx, self);
    float height = (canvas) ? canvas->getHeight() : 0;
    return JSValueMakeNumber(ctx, height);
}

WRAP_CLASS(Canvas) {
    //renderTo
    //getImageData
    JSLAddFunction(ctx, self, "setFilter", setFilter);
    //getFilter
    //setWrap
    //getWrap
    JSLAddFunction(ctx, self, "clear", clear);
    JSLAddFunction(ctx, self, "getWidth", getWidth);
    JSLAddFunction(ctx, self, "getHeight", getHeight);

    return self;
}
