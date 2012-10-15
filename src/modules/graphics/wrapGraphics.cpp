#include "WrapUtils.h"

#include <graphics/opengl/graphics.h>

using love::graphics::opengl::Graphics;

static Graphics* _g = NULL;

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
        _g->clear();
    }
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

    // getName
    JSLAddFunction(ctx, obj, "checkMode", checkMode);
    // saveState
    // restoreState
    JSLAddFunction(ctx, obj, "setMode", setMode);
    // getMode
    // toggleFullscreen
    JSLAddFunction(ctx, obj, "reset", reset);
    JSLAddFunction(ctx, obj, "clear", clear);
    JSLAddFunction(ctx, obj, "present", present);
    // setIcon
    JSLAddFunction(ctx, obj, "setCaption", setCaption);
    // getCaption
    // getWidth
    // getHeight
    // isCreated
    // getModes
    // setScissor
    // setScissor2
    // setScissor3
    // getScissor
    // defineStencil
    // useStencil
    // discardStencil
    // newImage
    // newImage2
    // newQuad
    // newFont
    // newSpriteBatch
    // newParticleSystem
    // newCanvas
    // newPixelEffect
    // setColor
    // getColor
    // setBackgroundColor
    // getBackgroundColor
    // setFont
    // getFont
    // setBlendMode
    // setColorMode
    // setDefaultImageFilter
    // getBlendMode
    // getColorMode
    // getDefaultImageFilter
    // setLineWidth
    // setLineStyle
    // setLine
    // getLineWidth
    // getLineStyle
    // setPointSize
    // setPointStyle
    // setPoint
    // getPointSize
    // getPointStyle
    // getMaxPointSize
    // print
    // printf
    // point
    // polyline
    // triangle
    // rectangle
    // quad
    // circle
    // arc
    // polygon
    // newScreenshot
    // push
    // pop
    // rotate
    // scale
    // translate
    // shear
    // drawTest
    // hasFocus

//    JSLWrapFunction(ctx, obj, "step", step);
//    JSLWrapFunction(ctx, obj, "sleep", sleep);
//    JSLWrapFunction(ctx, obj, "getDelta", getDelta);
//    JSLWrapFunction(ctx, obj, "getFPS", getFPS);
//    JSLWrapFunction(ctx, obj, "getTime", getTime);
//    JSLWrapFunction(ctx, obj, "getMicroTime", getMicroTime);

    return obj;
}
