#include "jscimpl.h"

#include "WrapUtils.h"

#include <JavaScriptCore/JavaScript.h>

#include <cstdio>

#define DECLARE_MODULE(name)               \
    extern JSValueRef __wrap_ ## name (    \
        JSContextRef ctx);                 \
                                           \
    static JSValueRef __init_ ## name (    \
        JSContextRef ctx,                  \
        JSObjectRef function,              \
        JSObjectRef self,                  \
        size_t argCount,                   \
        const JSValueRef args[],           \
        JSValueRef *exception)             \
    {                                      \
        return __wrap_ ## name (ctx);      \
    }

DECLARE_MODULE(timer);
DECLARE_MODULE(graphics);

JSValueRef Print(JSContextRef ctx,
                 JSObjectRef function,
                 JSObjectRef thisObject,
                 size_t argCount,
                 const JSValueRef args[],
                 JSValueRef *exception)
{
    for (size_t i = 0; i < argCount; i++) {
        JSStringRef str = JSValueToStringCopy(ctx, args[i], NULL);

        size_t size = JSStringGetMaximumUTF8CStringSize(str);
        char* s = new char[size];
        JSStringGetUTF8CString(str, s, size);

        JSStringRelease(str);

        if (i != 0) {
            printf(" ");
        }

        printf("%s", s);
    }

    printf("\n");

    return NULL;
}

std::string Stringify(JSContextRef ctx, JSValueRef value)
{
    if (JSValueIsUndefined(ctx, value)) {
        return "undefined";
    }

    JSStringRef str = JSValueToStringCopy(ctx, value, NULL);

    size_t size = JSStringGetMaximumUTF8CStringSize(str);
    char* s = new char[size];
    JSStringGetUTF8CString(str, s, size);
    JSStringRelease(str);

    std::string result(s);
    return result;
}

JSValueRef GetProperty(
    JSContextRef ctx,
    JSObjectRef obj,
    const char* name)
{
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSValueRef prop = JSObjectGetProperty(ctx, obj, nameStr, NULL);
    JSStringRelease(nameStr);
    return prop;
}

void ReportException(
    JSContextRef ctx,
    JSValueRef exception)
{
#if 0
    // XXX DEBUG
    {
        JSObjectRef obj = JSValueToObject(ctx, exception, NULL);
        JSPropertyNameArrayRef properties =
            JSObjectCopyPropertyNames(ctx, obj);

        for (size_t i = 0; i < JSPropertyNameArrayGetCount(properties); i++) {
            JSStringRef str = JSPropertyNameArrayGetNameAtIndex(properties, i);

            size_t size = JSStringGetMaximumUTF8CStringSize(str);
            char* s = new char[size];
            JSStringGetUTF8CString(str, s, size);

            JSValueRef prop = JSObjectGetProperty(ctx, obj, str, NULL);

            printf("--> %s (%s)\n", s, Stringify(ctx, prop).c_str());
        }
    }
#endif

    const std::string& message = Stringify(ctx, exception);

    JSObjectRef obj = JSValueToObject(ctx, exception, NULL);
    JSValueRef sourceURL = GetProperty(ctx, obj, "sourceURL");
    JSValueRef line = GetProperty(ctx, obj, "line");

    printf("%s (%s:%s)\n",
           message.c_str(),
           Stringify(ctx, sourceURL).c_str(),
           Stringify(ctx, line).c_str());
}

void InitContext(JSContextRef ctx)
{
    {
        JSStringRef name = JSStringCreateWithUTF8CString("print");
        JSObjectRef func =
            JSObjectMakeFunctionWithCallback(ctx, name, Print);
        JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func,
                            kJSPropertyAttributeNone, /*exception*/ NULL);
        JSStringRelease(name);
    }

    {
        JSStringRef name = JSStringCreateWithUTF8CString("__init_timer");
        JSObjectRef func =
            JSObjectMakeFunctionWithCallback(ctx, name, __init_timer);
        JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func,
                            kJSPropertyAttributeNone, /*exception*/ NULL);
        JSStringRelease(name);
    }

    {
        JSStringRef name = JSStringCreateWithUTF8CString("__init_graphics");
        JSObjectRef func =
            JSObjectMakeFunctionWithCallback(ctx, name, __init_graphics);
        JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func,
                            kJSPropertyAttributeNone, /*exception*/ NULL);
        JSStringRelease(name);
    }
}

int JSCMain(const std::string& str)
{
    printf("JavaScriptCore!\n");

    JSGlobalContextRef ctx = JSGlobalContextCreate(NULL);

    InitContext(ctx);

    JSStringRef script = JSStringCreateWithUTF8CString(str.c_str());
    JSStringRef name = JSStringCreateWithUTF8CString("boot.lua");
    JSValueRef exception = NULL;
    JSValueRef result = JSEvaluateScript(ctx, script, NULL, name, 0, &exception);

    if (result and not JSValueIsUndefined(ctx, result)) {
        JSStringRef str = JSValueToStringCopy(ctx, result, NULL);

        size_t size = JSStringGetMaximumUTF8CStringSize(str);
        char* s = new char[size];
        JSStringGetUTF8CString(str, s, size);
        printf("result: %s\n", s);
    }

    if (exception) {
        ReportException(ctx, exception);
    }

    JSGlobalContextRelease(ctx);

    return 0;
}
