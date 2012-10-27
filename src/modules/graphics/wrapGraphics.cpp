#include "WrapUtils.h"

#include <common/types.h>
#include <graphics/opengl/Graphics.h>
#include <graphics/Drawable.h>
#include <graphics/Image.h>
#include <image/ImageData.h>

using love::graphics::opengl::Graphics;
using love::graphics::Drawable;
using love::graphics::Image;
using love::image::ImageData;

static Graphics* _g = NULL;

DECLARE_TYPE(Drawable,  love::GRAPHICS_DRAWABLE_T);
DECLARE_TYPE(Image,     love::GRAPHICS_IMAGE_T);
//DECLARE_TYPE(ImageData, love::IMAGE_IMAGE_DATA_T);

WRAP_FUNCTION(checkMode)
{
    if (argCount == 3) {
        int width       = JSLGetNumber(ctx, args[0]);
        int height      = JSLGetNumber(ctx, args[1]);
        bool fullscreen = JSLGetBoolean(ctx, args[2]);

        bool result = _g->checkMode(width, height, fullscreen);
        return JSValueMakeBoolean(ctx, result);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setMode)
{
    if (argCount == 5) {
        int width       = JSLGetNumber(ctx, args[0]);
        int height      = JSLGetNumber(ctx, args[1]);
        bool fullscreen = JSLGetBoolean(ctx, args[2]);
        bool vsync      = JSLGetBoolean(ctx, args[3]);
        int fsaa        = JSLGetNumber(ctx, args[4]);

        bool result = _g->setMode(width, height, fullscreen, vsync, fsaa);
        return JSValueMakeBoolean(ctx, result);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(reset)
{
    if (argCount == 0) {
        _g->clear();
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(clear)
{
    if (argCount == 0) {
        _g->clear();
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(present)
{
    if (argCount == 0) {
        _g->present();
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setColor)
{
    if (argCount == 4) {
        love::graphics::Color c;
        c.r = JSLGetNumber(ctx, args[0]);
        c.g = JSLGetNumber(ctx, args[1]);
        c.b = JSLGetNumber(ctx, args[2]);
        c.a = JSLGetNumber(ctx, args[3], 255.0);

        _g->setColor(c);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setBackgroundColor)
{
    if (argCount == 3) {
        love::graphics::Color c;
        c.r = JSLGetNumber(ctx, args[0]);
        c.g = JSLGetNumber(ctx, args[1]);
        c.b = JSLGetNumber(ctx, args[2]);
        c.a = 255;

        _g->setBackgroundColor(c);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(draw)
{
    if (argCount == 10) {
        Drawable* drawable = JSLExtractObject<Drawable>(ctx, args[0]);

        if (drawable) {
            float x = JSLGetNumber(ctx, args[1]);
            float y = JSLGetNumber(ctx, args[2]);
            float angle = JSLGetNumber(ctx, args[3]);
            float sx = JSLGetNumber(ctx, args[4]);
            float sy = JSLGetNumber(ctx, args[5]);
            float ox = JSLGetNumber(ctx, args[6]);
            float oy = JSLGetNumber(ctx, args[7]);
            float kx = JSLGetNumber(ctx, args[8]);
            float ky = JSLGetNumber(ctx, args[9]);

            drawable->draw(x, y, angle, sx, sy, ox, oy, kx, ky);
        }
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(newImage)
{
    // TODO: overloads:
    //   - string filename
    //   - file file

    if (argCount == 1) {
        JSObjectRef arg0 = JSValueToObject(ctx, args[0], NULL);

        if (not JSLObjectIsA(arg0, love::IMAGE_IMAGE_DATA_T)) {
            printf("ERROR: Invalid image data\n");
            goto undefined;
        }

        ImageData* imageData = JSLExtractObject<ImageData>(arg0);
        Image* image = NULL;

        try {
            image = _g->newImage(imageData);
        } catch (love::Exception& e) {
            printf("ERROR: Couldn't create image: %s\n", e.what());
            goto undefined;
        }

        JSObjectRef obj = JSLCreateObject(ctx, image,
                                          love::GRAPHICS_IMAGE_T);
        return obj;
    }

undefined:
    return JSValueMakeUndefined(ctx);
}


WRAP_FUNCTION(setCaption)
{
    if (argCount == 1) {
        std::string caption = JSLGetString(ctx, args[0]);
        _g->setCaption(caption.c_str());
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_MODULE(graphics)
{
    printf("INIT graphics\n");

    if (not _g) {
        _g = new Graphics();
    } else {
        printf("ERROR: already initialized graphics\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "checkMode", checkMode);
    JSLAddFunction(ctx, obj, "setMode", setMode);
//    "getMode"
//    "toggleFullscreen"
    JSLAddFunction(ctx, obj, "reset", reset);
    JSLAddFunction(ctx, obj, "clear", clear);
    JSLAddFunction(ctx, obj, "present", present);

    JSLAddFunction(ctx, obj, "newImage", newImage);
//    "newQuad"
//    "newFont1"
//    "newImageFont"
//    "newSpriteBatch"
//    "newParticleSystem"
//    "newCanvas"
//    "newPixelEffect"

    JSLAddFunction(ctx, obj, "setColor", setColor);
//    "getColor"
    JSLAddFunction(ctx, obj, "setBackgroundColor", setBackgroundColor);
//    "getBackgroundColor"

//    "setFont"
//    "getFont"

//    "setBlendMode"
//    "setColorMode"
//    "setDefaultImageFilter"
//    "getBlendMode"
//    "getColorMode"
//    "getDefaultImageFilter"
//    "setLineWidth"
//    "setLineStyle"
//    "setLine"
//    "getLineWidth"
//    "getLineStyle"
//    "setPointSize"
//    "setPointStyle"
//    "setPoint"
//    "getPointSize"
//    "getPointStyle"
//    "getMaxPointSize"
//    "newScreenshot"
//    "setCanvas"
//    "getCanvas"

//    "setPixelEffect"
//    "getPixelEffect"

//    "isSupported"

    JSLAddFunction(ctx, obj, "draw", draw);
//    "drawq"
//    "drawTest"

//    "print1"
//    "printf1"

//    "setCaption"
    JSLAddFunction(ctx, obj, "setCaption", setCaption);
//    "getCaption"

//    "setIcon"

//    "getWidth"
//    "getHeight"

//    "isCreated"

//    "getModes"

//    "setScissor"
//    "getScissor"

//    "newStencil"
//    "setStencil"
//    "setInvertedStencil"

//    "point"
//    "line"
//    "triangle"
//    "rectangle"
//    "quad"
//    "circle"
//    "arc"

//    "polygon"

//    "push"
//    "pop"
//    "rotate"
//    "scale"
//    "translate"
//    "shear"

//    "hasFocus"

//    JSLWrapFunction(ctx, obj, "step", step);
//    JSLWrapFunction(ctx, obj, "sleep", sleep);
//    JSLWrapFunction(ctx, obj, "getDelta", getDelta);
//    JSLWrapFunction(ctx, obj, "getFPS", getFPS);
//    JSLWrapFunction(ctx, obj, "getTime", getTime);
//    JSLWrapFunction(ctx, obj, "getMicroTime", getMicroTime);

    return obj;
}
