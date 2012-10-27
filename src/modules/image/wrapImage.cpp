#include "WrapUtils.h"

#include <common/Data.h>
#include <common/types.h>
#include <image/ImageData.h>
#include <image/devil/Image.h>

using love::Data;
using love::image::ImageData;
using love::image::devil::Image;

static Image* _i = NULL;

DECLARE_TYPE(Data,      love::DATA_T); // XXX move this
DECLARE_TYPE(ImageData, love::IMAGE_IMAGE_DATA_T);

WRAP_FUNCTION(newImageData)
{
    // TODO: overloads:
    //   - int width, int height
    //   - string filename
    //   - file file

    if (argCount == 1) {
        JSObjectRef arg0 = JSValueToObject(ctx, args[0], NULL);

        if (not JSLObjectIsA(arg0, love::DATA_T)) {
            printf("ERROR: Invalid data\n");
            goto undefined;
        }

        Data* data = JSLExtractObject<Data>(arg0);
        ImageData* imageData = NULL;

        try {
            imageData = _i->newImageData(data);
        } catch (love::Exception& e) {
            printf("ERROR: Couldn't create image data: %s\n", e.what());
            goto undefined;
        }

        JSObjectRef obj = JSLCreateObject(ctx, imageData,
                                          love::IMAGE_IMAGE_DATA_T);
        return obj;
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_MODULE(image)
{
    printf("INIT image\n");

    if (not _i) {
        _i = new Image();
    } else {
        printf("ERROR: already initialized image\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "newImageData", newImageData);
    // newImageData(file)
    // newImageData(width, height)
    // newImageData(width, height, dataPtr)

    return obj;
}
