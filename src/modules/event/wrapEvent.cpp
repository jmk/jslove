#include "JSLPrivateAccess.h"
#include "WrapUtils.h"

#include <event/sdl/Event.h>

using love::Variant;
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
        int count = JSLPrivateAccess::Message::nargs(m);
        JSValueRef values[count + 1];

        values[0] = JSLMakeStringValue(
            ctx, JSLPrivateAccess::Message::name(m));

        for (int i = 0; i < count; i++) {
            Variant* v = JSLPrivateAccess::Message::arg(m, i);
            values[i + 1] = JSLPrivateAccess::Variant::toValue(v, ctx);
        }

        m->release();
        return JSObjectMakeArray(ctx, count + 1, values, NULL);
    }

    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(quit)
{
    Message *m = new Message("quit");
    _e->push(m);
    m->release();

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
    // wait
    // push
    // clear
    JSLAddFunction(ctx, obj, "quit", quit);

    return obj;
}


