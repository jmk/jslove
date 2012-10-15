#ifndef __jslove__wraputils__
#define __jslove__wraputils__

#include <JavaScriptCore/JavaScript.h>

#include <string>

void JSLAddFunction(
    JSContextRef ctx,
    JSObjectRef obj,
    const char* name,
    JSObjectCallAsFunctionCallback callback);

bool JSLGetBoolean(JSContextRef, JSValueRef);
double JSLGetNumber(JSContextRef, JSValueRef);
std::string JSLGetString(JSContextRef, JSValueRef);

#define WRAP_FUNCTION(name)         \
    static JSValueRef name (        \
        JSContextRef ctx,           \
        JSObjectRef function,       \
        JSObjectRef self,           \
        size_t argCount,            \
        const JSValueRef args[],    \
        JSValueRef *exception)      \

#define WRAP_MODULE(name)           \
    JSValueRef __wrap_ ## name (    \
        JSContextRef ctx)

#endif
