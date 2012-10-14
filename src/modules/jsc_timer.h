#ifndef __jslove__jsc_timer__
#define __jslove__jsc_timer__

#include <JavaScriptCore/JavaScript.h>

void AddFunction(
    JSContextRef ctx,
    JSObjectRef obj,
    const char* name,
    JSObjectCallAsFunctionCallback callback)
{
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSObjectRef func =
        JSObjectMakeFunctionWithCallback(ctx, nameStr, callback);
    JSObjectSetProperty(ctx, obj, nameStr, func,
                        kJSPropertyAttributeNone,
                        /*exception*/ NULL);

    JSStringRelease(nameStr);
}

#define WRAP_FUNCTION(name)         \
    static JSValueRef name (        \
        JSContextRef ctx,           \
        JSObjectRef function,       \
        JSObjectRef self,           \
        size_t argCount,            \
        const JSValueRef args[],    \
        JSValueRef *exception)      \

#define WRAP_MODULE(name)           \
    JSValueRef __init_ ## name (    \
        JSContextRef ctx,           \
        JSObjectRef function,       \
        JSObjectRef self,           \
        size_t argCount,            \
        const JSValueRef args[],    \
        JSValueRef *exception)      \

#endif /* defined(__jslove__jsc_timer__) */
