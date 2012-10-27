#include "WrapUtils.h"

#include <timer/sdl/timer.h>

#include <cstdio>

using love::timer::sdl::Timer;

static Timer* _t = NULL;

WRAP_FUNCTION(step)
{
    if (argCount == 0) {
        _t->step();
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(sleep)
{
    if (argCount == 1) {
        _t->sleep(JSLGetNumber(ctx, args[0]));
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(getDelta)
{
    return JSValueMakeNumber(ctx, _t->getDelta());
}

WRAP_FUNCTION(getFPS)
{
    return JSValueMakeNumber(ctx, _t->getFPS());
}

WRAP_FUNCTION(getTime)
{
    return JSValueMakeNumber(ctx, _t->getTime());
}

WRAP_FUNCTION(getMicroTime)
{
    return JSValueMakeNumber(ctx, _t->getMicroTime());
}

WRAP_MODULE(timer)
{
    printf("INIT timer\n");

    if (not _t) {
        _t = new Timer();
    } else {
        printf("ERROR: already initialized timer\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "step", step);
    JSLAddFunction(ctx, obj, "sleep", sleep);
    JSLAddFunction(ctx, obj, "getDelta", getDelta);
    JSLAddFunction(ctx, obj, "getFPS", getFPS);
    JSLAddFunction(ctx, obj, "getTime", getTime);
    JSLAddFunction(ctx, obj, "getMicroTime", getMicroTime);

    return obj;
}
