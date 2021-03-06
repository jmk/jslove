#include "WrapUtils.h"

#include <common/Data.h>
#include <common/types.h>
#include <filesystem/physfs/File.h>
#include <filesystem/File.h>
#include <font/freetype/Font.h>
#include <font/Rasterizer.h>
#include <graphics/opengl/Font.h>
#include <graphics/opengl/Graphics.h>
#include <graphics/Drawable.h>
#include <graphics/Image.h>
#include <image/devil/ImageData.h>
#include <image/ImageData.h>

using love::Data;
using love::Exception;
using love::filesystem::File;
using love::font::Rasterizer;
using love::graphics::Drawable;
using love::graphics::DrawQable;
using love::graphics::Image;
using love::graphics::Quad;
using love::graphics::opengl::Canvas;
using love::graphics::opengl::Font;
using love::graphics::opengl::Graphics;
using love::image::ImageData;

static Graphics* _g = NULL;

// XXX: These should be wrapped as classes.
DECLARE_TYPE(Drawable,  love::GRAPHICS_DRAWABLE_T);
DECLARE_TYPE(DrawQable, love::GRAPHICS_DRAWQABLE_T);

DECLARE_CLASS(Canvas,     love::GRAPHICS_CANVAS_T);
DECLARE_CLASS(Font,       love::GRAPHICS_FONT_T);
DECLARE_CLASS(Image,      love::GRAPHICS_IMAGE_T);
DECLARE_CLASS(Quad,       love::GRAPHICS_QUAD_T);
DECLARE_CLASS(Rasterizer, love::FONT_RASTERIZER_T);

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
    _g->clear();
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(clear)
{
    _g->clear();
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(present)
{
    _g->present();
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(newCanvas)
{
    // TODO: overloads:
    //   - filename string
    //   - file

    if (argCount == 2) {
        double width = JSLGetNumber(ctx, args[0]);
        double height = JSLGetNumber(ctx, args[1]);

        glGetError(); // clear opengl error flag

        Canvas* canvas = NULL;

        try {
            canvas = _g->newCanvas(width, height);
        } catch (Exception& e) {
            printf("ERROR: Couldn't create canvas: %s\n", e.what());
            goto undefined;
        }

        JSObjectRef obj = JSLCreateObject(ctx, canvas);
        return obj;
    }

undefined:
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

WRAP_FUNCTION(setFont)
{
    if (argCount == 1) {
        Font* font = JSLExtractObject<Font>(ctx, args[0]);
        if (font) {
            _g->setFont(font);
        }
    } else {
        printf("ERROR: Invalid font\n");
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(getFont)
{
    Font* font = _g->getFont();

    if (font) {
        font->retain();
        return JSLCreateObject(ctx, font);
    } else {
        return JSValueMakeUndefined(ctx);
    }
}

WRAP_FUNCTION(draw)
{
    if (argCount >= 3 and argCount <= 10) {
        Drawable* drawable = JSLExtractObject<Drawable>(ctx, args[0]);

        if (drawable) {
            float x = JSLGetNumber(ctx, args[1]);
            float y = JSLGetNumber(ctx, args[2]);

            // Optional args.
            float r  = (argCount > 3) ? JSLGetNumber(ctx, args[3]) : 0.0;
            float sx = (argCount > 4) ? JSLGetNumber(ctx, args[4]) : 1.0;
            float sy = (argCount > 5) ? JSLGetNumber(ctx, args[5]) : sx;
            float ox = (argCount > 6) ? JSLGetNumber(ctx, args[6]) : 0.0;
            float oy = (argCount > 7) ? JSLGetNumber(ctx, args[7]) : 0.0;
            float kx = (argCount > 8) ? JSLGetNumber(ctx, args[8]) : 0.0;
            float ky = (argCount > 9) ? JSLGetNumber(ctx, args[9]) : 0.0;

            drawable->draw(x, y, r, sx, sy, ox, oy, kx, ky);
        }
    } else {
        printf("ERROR: Invalid arguments to draw()\n");
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setCanvas)
{
    // Discard stencil testing.
    _g->discardStencil();

    if (argCount == 0) {
        // If called with no args, reset to default buffer.
        Canvas::bindDefaultCanvas();
    } else if (argCount == 1) {
        // If called with null, reset to default buffer.
        if (JSValueIsUndefined(ctx, args[0])) {
            Canvas::bindDefaultCanvas();
        } else {
            Canvas* canvas = JSLExtractObject<Canvas>(ctx, args[0]);
            if (canvas) {
                // This unbinds the previous FBO.
                canvas->startGrab();
            }
        }
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(getCanvas)
{
    Canvas* canvas = Canvas::current;

    if (canvas) {
        canvas->retain();
        return JSLCreateObject(ctx, canvas);
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(drawq)
{
    if (argCount >= 3 and argCount <= 11) {
        DrawQable* drawqable = JSLExtractObject<DrawQable>(ctx, args[0]);
        Quad* quad = JSLExtractObject<Quad>(ctx, args[1]);

        if (drawqable and quad) {
            float x = JSLGetNumber(ctx, args[2]);
            float y = JSLGetNumber(ctx, args[3]);

            // Optional args.
            float r  = (argCount > 4) ? JSLGetNumber(ctx, args[4]) : 0.0;
            float sx = (argCount > 5) ? JSLGetNumber(ctx, args[5]) : 1.0;
            float sy = (argCount > 6) ? JSLGetNumber(ctx, args[6]) : sx;
            float ox = (argCount > 7) ? JSLGetNumber(ctx, args[7]) : 0.0;
            float oy = (argCount > 8) ? JSLGetNumber(ctx, args[8]) : 0.0;
            float kx = (argCount > 9) ? JSLGetNumber(ctx, args[9]) : 0.0;
            float ky = (argCount > 10) ? JSLGetNumber(ctx, args[10]) : 0.0;

            drawqable->drawq(quad, x, y, r, sx, sy, ox, oy, kx, ky);
        } else {
            printf("ERROR: Invalid quad\n");
        }
    } else {
        printf("ERROR: Invalid arguments to drawq()\n");
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(print)
{
    // text, x, y, r, sx, sy, ox, oy, kx, ky
    if (argCount >= 3 and argCount <= 10) {
        std::string text = JSLGetString(ctx, args[0]);
        float x = JSLGetNumber(ctx, args[1]);
        float y = JSLGetNumber(ctx, args[2]);

        // Optional args.
        float r  = (argCount > 3) ? JSLGetNumber(ctx, args[3]) : 0.0;
        float sx = (argCount > 4) ? JSLGetNumber(ctx, args[4]) : 1.0;
        float sy = (argCount > 5) ? JSLGetNumber(ctx, args[5]) : sx;
        float ox = (argCount > 6) ? JSLGetNumber(ctx, args[6]) : 0.0;
        float oy = (argCount > 7) ? JSLGetNumber(ctx, args[7]) : 0.0;
        float kx = (argCount > 8) ? JSLGetNumber(ctx, args[8]) : 0.0;
        float ky = (argCount > 9) ? JSLGetNumber(ctx, args[9]) : 0.0;

        try {
            _g->print(text.c_str(), x, y, r, sx, sy, ox, oy, kx, ky);
        } catch (love::Exception& e) {
            printf("ERROR: Decoding error: %s", e.what());
        }
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(printf)
{
    if (argCount >= 4 and argCount <= 5) {
        std::string text = JSLGetString(ctx, args[0]);
        float x = JSLGetNumber(ctx, args[1]);
        float y = JSLGetNumber(ctx, args[2]);
        float limit = JSLGetNumber(ctx, args[3]);

        Graphics::AlignMode align = Graphics::ALIGN_LEFT;

        if (argCount == 5) {
            std::string alignStr = JSLGetString(ctx, args[4]);
            if (not Graphics::getConstant(alignStr.c_str(), align)) {
                printf("ERROR: Invalid alignment: '%s'\n", alignStr.c_str());
                goto undefined;
            }
        }

        try {
            _g->printf(text.c_str(), x, y, limit, align);
        } catch (love::Exception& e) {
            printf("ERROR: Decoding error: %s", e.what());
        }
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(newImage)
{
    // TODO: overloads:
    //   - file object

    if (argCount == 1) {
        JSObjectRef arg0 = JSValueToObject(ctx, args[0], NULL);
        ImageData* imageData = NULL;

        if (JSLObjectIsA<ImageData>(arg0)) {
            // Argument is image data; use it directly.
            imageData = JSLExtractObject<ImageData>(arg0);
        } else {
            // Argument is other type; attempt to create file object to read
            // into image data.
            File* file = NULL;

            if (JSLObjectIsA<File>(arg0)) {
                file = JSLExtractObject<File>(arg0);
            } else if (JSValueIsString(ctx, args[0])) {
                // If a string was passed in, open as file object.
                std::string filename = JSLGetString(ctx, args[0]);
                file = new love::filesystem::physfs::File(filename);
            }

            if (not file) {
                printf("ERROR: Invalid argument type to newImage()\n");
                goto undefined;
            }

            imageData = new love::image::devil::ImageData(file);
        }

        if (not imageData) {
            printf("ERROR: Failed to create image data\n");
            goto undefined;
        }

        Image* image = NULL;

        try {
            image = _g->newImage(imageData);
        } catch (love::Exception& e) {
            printf("ERROR: Couldn't create image: %s\n", e.what());
            goto undefined;
        }

        JSObjectRef obj = JSLCreateObject(ctx, image);
        return obj;
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(newQuad)
{
    //x, y, width, height, sw, sh
    if (argCount == 6) {
        float x  = JSLGetNumber(ctx, args[0]);
        float y  = JSLGetNumber(ctx, args[1]);
        float w  = JSLGetNumber(ctx, args[2]);
        float h  = JSLGetNumber(ctx, args[3]);
        float sw = JSLGetNumber(ctx, args[4]);
        float sh = JSLGetNumber(ctx, args[5]);

        Quad* quad = NULL;

        try {
            quad = _g->newQuad(x, y, w, h, sw, sh);
        } catch (Exception& e) {
            printf("ERROR: Couldn't create quad: %s\n", e.what());
            goto undefined;
        }

        return JSLCreateObject(ctx, quad);
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

// XXX
extern love::font::freetype::Font* _GetGlobalFontHack();

WRAP_FUNCTION(newFont)
{
    Data* data = NULL;

    // TODO overloads:
    //   - file, (size)
    //   - data, (size)

    if (argCount >= 1) {
        JSObjectRef arg0 = JSValueToObject(ctx, args[0], NULL);

        if (JSLObjectIsA<Data>(arg0)) {
            // Argument is image data; use it directly.
            data = JSLExtractObject<Data>(arg0);
        } else {
            // Argument is other type; attempt to create file object to read
            // into image data.
            File* file = NULL;

            if (JSLObjectIsA<File>(arg0)) {
                file = JSLExtractObject<File>(arg0);
            } else if (JSValueIsString(ctx, args[0])) {
                // If a string was passed in, open as file object.
                std::string filename = JSLGetString(ctx, args[0]);
                file = new love::filesystem::physfs::File(filename);
            }

            if (not file) {
                printf("ERROR: Invalid argument type to newFont()\n");
                goto undefined;
            }

            // Read font data from disk.
            try {
                data = file->read();
            } catch (love::Exception& e) {
                printf("ERROR: Couldn't read font: %s\n",
                       file->getFilename().c_str());
                goto undefined;
            }
        }

        if (not data) {
            printf("ERROR: Invalid font data\n");
            goto undefined;
        }

        int size = 12;  // Default as stated in docs.
        if (argCount >= 2) {
            size = JSLGetNumber(ctx, args[1], size);
        }

        love::font::freetype::Font* f = _GetGlobalFontHack();
        if (not f) {
            printf("ERROR: Font module not initialized\n");
            goto undefined;
        }

        Rasterizer* rasterizer = f->newRasterizer(data, size);
        if (not rasterizer) {
            printf("ERROR: Failed to create font rasterizer\n");
            goto undefined;
        }

        Font* font = _g->newFont(rasterizer);
        if (not font) {
            printf("ERROR: Couldn't create font\n");
            goto undefined;
        }

        return JSLCreateObject(ctx, font);
    }

undefined:
    if (data) {
        data->release();
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

WRAP_FUNCTION(getWidth)
{
    int width = _g->getWidth();
    return JSValueMakeNumber(ctx, width);
}

WRAP_FUNCTION(getHeight)
{
    int height = _g->getHeight();
    return JSValueMakeNumber(ctx, height);
}

WRAP_FUNCTION(push)
{
    _g->push();
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(pop)
{
    _g->pop();
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
    JSLAddFunction(ctx, obj, "newQuad", newQuad);
    JSLAddFunction(ctx, obj, "newFont", newFont);
//    "newImageFont"
//    "newSpriteBatch"
//    "newParticleSystem"
    JSLAddFunction(ctx, obj, "newCanvas", newCanvas);
//    "newPixelEffect"

    JSLAddFunction(ctx, obj, "setColor", setColor);
//    "getColor"
    JSLAddFunction(ctx, obj, "setBackgroundColor", setBackgroundColor);
//    "getBackgroundColor"

    JSLAddFunction(ctx, obj, "setFont", setFont);
    JSLAddFunction(ctx, obj, "getFont", getFont);

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

    JSLAddFunction(ctx, obj, "setCanvas", setCanvas);
    JSLAddFunction(ctx, obj, "getCanvas", getCanvas);
//    "setPixelEffect"
//    "getPixelEffect"

//    "isSupported"

    JSLAddFunction(ctx, obj, "draw", draw);
    JSLAddFunction(ctx, obj, "drawq", drawq);
//    "drawTest"

    JSLAddFunction(ctx, obj, "print", print);
    JSLAddFunction(ctx, obj, "printf", printf);

//    "setCaption"
    JSLAddFunction(ctx, obj, "setCaption", setCaption);
//    "getCaption"

//    "setIcon"

    JSLAddFunction(ctx, obj, "getWidth", getWidth);
    JSLAddFunction(ctx, obj, "getHeight", getHeight);

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

    JSLAddFunction(ctx, obj, "push", push);
    JSLAddFunction(ctx, obj, "pop", pop);
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
