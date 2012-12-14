#include "WrapUtils.h"

#include <font/Rasterizer.h>

using love::font::Rasterizer;

//DECLARE_CLASS(Rasterizer, love::FONT_RASTERIZER_T);

WRAP_CLASS(Rasterizer) {
    return self;
}
