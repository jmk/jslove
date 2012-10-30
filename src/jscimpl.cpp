#include "jscimpl.h"

#include "WrapUtils.h"

#include <JavaScriptCore/JavaScript.h>

#include <cstdio>

#define DECLARE_MODULE(name)                    \
    extern JSValueRef __wrap_module_ ## name (  \
        JSContextRef ctx);                      \
                                                \
    static JSValueRef __init_ ## name (         \
        JSContextRef ctx,                       \
        JSObjectRef function,                   \
        JSObjectRef self,                       \
        size_t argCount,                        \
        const JSValueRef args[],                \
        JSValueRef *exception)                  \
    {                                           \
        return __wrap_module_ ## name (ctx);    \
    }

DECLARE_MODULE(event);
DECLARE_MODULE(image);
DECLARE_MODULE(filesystem);
DECLARE_MODULE(graphics);
DECLARE_MODULE(timer);

static JSValueRef
Print(
    JSContextRef ctx,
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

static std::string
Stringify(JSContextRef ctx, JSValueRef value)
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

static JSValueRef
GetProperty(
    JSContextRef ctx,
    JSObjectRef obj,
    const char* name)
{
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSValueRef prop = JSObjectGetProperty(ctx, obj, nameStr, NULL);
    JSStringRelease(nameStr);
    return prop;
}

static void
ReportException(
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

static void
SetGlobal(JSContextRef ctx, const char* name, JSObjectRef obj)
{
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), nameStr, obj,
                        kJSPropertyAttributeNone, /*exception*/ NULL);
    JSStringRelease(nameStr);
}

static void
SetGlobalFunction(
JSContextRef ctx, const char* name, JSObjectCallAsFunctionCallback func)
{
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSObjectRef funcObj = JSObjectMakeFunctionWithCallback(ctx, nameStr, func);
    SetGlobal(ctx, name, funcObj);
    JSStringRelease(nameStr);
}

static void
InitContext(JSContextRef ctx)
{
    SetGlobalFunction(ctx, "print", Print);

    // Module initializers
    SetGlobalFunction(ctx, "__init_event", __init_event);
    SetGlobalFunction(ctx, "__init_filesystem", __init_filesystem);
    SetGlobalFunction(ctx, "__init_graphics", __init_graphics);
    SetGlobalFunction(ctx, "__init_image", __init_image);
    SetGlobalFunction(ctx, "__init_timer", __init_timer);
}

static void
InitArgs(
    JSContextRef ctx,
    int argc,
    char** argv)
{
    JSValueRef values[argc];

    for (int i = 0; i < argc; i++) {
        JSStringRef arg = JSStringCreateWithUTF8CString(argv[i]);
        values[i] = JSValueMakeString(ctx, arg);
        JSStringRelease(arg);
    }

    SetGlobal(ctx, "__argv", JSObjectMakeArray(ctx, argc, values, NULL));
}

int JSCMain(
    const std::string& bootScript,
    int argc,
    char** argv)
{
    printf("JavaScriptCore!\n");

    JSGlobalContextRef ctx = JSGlobalContextCreate(NULL);

    InitContext(ctx);
    InitArgs(ctx, argc, argv);

    JSStringRef script = JSStringCreateWithUTF8CString(bootScript.c_str());
    JSStringRef name = JSStringCreateWithUTF8CString("boot.js");
    JSValueRef exception = NULL;
    JSValueRef result = JSEvaluateScript(ctx, script, NULL, name, 0, &exception);

    if (result and not JSValueIsUndefined(ctx, result)) {
        JSStringRef str = JSValueToStringCopy(ctx, result, NULL);

        size_t size = JSStringGetMaximumUTF8CStringSize(str);
        char* s = new char[size];
        JSStringGetUTF8CString(str, s, size);
        printf("end! result: %s\n", s);
    }

    if (exception) {
        ReportException(ctx, exception);
    }

    JSGlobalContextRelease(ctx);

    return 0;
}
