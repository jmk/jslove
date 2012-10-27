#include "WrapUtils.h"

#include <event/sdl/Event.h>

using love::event::Message;
using love::event::sdl::Event;

static Event* _e = NULL;

WRAP_FUNCTION(pump)
{
    _e->pump();

    // No return value.
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(poll)
{
    Message *m = NULL;

    if (_e->poll(m))
    {
        // XXX: Return array of objects converted from message
//        int args = m->toLua(L);
        m->release();
//        return args;
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_MODULE(event)
{
    printf("INIT event\n");

    if (not _e) {
        _e = new Event();
    } else {
        printf("ERROR: already initialized event\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "pump", pump);
    JSLAddFunction(ctx, obj, "poll", poll);

    return obj;
}


