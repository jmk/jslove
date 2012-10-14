#include "jsc_timer.h"

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
        if (JSValueIsNumber(ctx, args[0])) {
            double seconds = JSValueToNumber(ctx, args[0], NULL);
            _t->sleep(seconds);
        }
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
    printf("INIT TIMER JavaScriptCore\n");

    if (not _t) {
        _t = new Timer();
    } else {
        printf("ERROR: already initialized\n");
    }

    JSClassDefinition def = kJSClassDefinitionEmpty;
//    def.getProperty = myClassGetProperty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    AddFunction(ctx, obj, "step", step);
    AddFunction(ctx, obj, "sleep", sleep);
    AddFunction(ctx, obj, "getDelta", getDelta);
    AddFunction(ctx, obj, "getFPS", getFPS);
    AddFunction(ctx, obj, "getTime", getTime);
    AddFunction(ctx, obj, "getMicroTime", getMicroTime);

    return obj;
}
