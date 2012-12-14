#include "WrapUtils.h"

#include <common/types.h>
#include <font/freetype/Font.h>

using love::font::freetype::Font;

static Font* _f = NULL;

// XXX: We should have a better way to handle this.
Font* _GetGlobalFontHack() {
    return _f;
}

WRAP_MODULE(font)
{
    printf("INIT font\n");

    if (not _f) {
        _f = new Font();
    } else {
        printf("ERROR: already initialized font\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    // newFontData
    // newGlyphData
    // newRasterizer

    return obj;
}
