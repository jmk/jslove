#include "WrapUtils.h"

#include <common/types.h>
#include <keyboard/sdl/Keyboard.h>

using love::keyboard::sdl::Keyboard;

static Keyboard* _k = NULL;

WRAP_FUNCTION(isDown)
{
    bool result = false;

    int counter = 0;
    Keyboard::Key *keyList = new Keyboard::Key[argCount + 1];

    for (int i = 0; i < argCount; ++i) {
        std::string key = JSLGetString(ctx, args[i]);
        Keyboard::Key k;

        if (Keyboard::getConstant(key.c_str(), k)) {
            keyList[counter++] = k;
        }
    }

    keyList[counter] = Keyboard::KEY_MAX_ENUM;

    result = _k->isDown(keyList);
    delete[] keyList;

    return JSValueMakeBoolean(ctx, result);
}

WRAP_FUNCTION(setKeyRepeat)
{
    if (argCount == 0) {
        // Disables key repeat.
        _k->setKeyRepeat(0, 0);
    } else if (argCount == 2) {
        int delay = Keyboard::DEFAULT;
        int interval = Keyboard::DEFAULT;

        if (JSValueIsNumber(ctx, args[0])) {
            delay = JSValueToNumber(ctx, args[0], NULL) * 1000 + 0.5;
        }
        if (JSValueIsNumber(ctx, args[1])) {
            interval = JSValueToNumber(ctx, args[0], NULL) * 1000 + 0.5;
        }

        _k->setKeyRepeat(delay, interval);
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_MODULE(keyboard)
{
    printf("INIT keyboard\n");

    if (not _k) {
        _k = new Keyboard();
    } else {
        printf("ERROR: already initialized keyboard\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "isDown", isDown);
    JSLAddFunction(ctx, obj, "setKeyRepeat", setKeyRepeat);
    // getKeyRepeat

    return obj;
}

