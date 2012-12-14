#include "jscimpl.h"

#include "WrapUtils.h"

#include <JavaScriptCore/JavaScript.h>

#include <cstdio>

// Forward declaration; implemented below.
static void SetGlobalFunction(JSContextRef ctx, const char* name,
                              JSObjectCallAsFunctionCallback func);

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
DECLARE_MODULE(filesystem);
DECLARE_MODULE(font);
DECLARE_MODULE(graphics);
DECLARE_MODULE(image);
DECLARE_MODULE(keyboard);
DECLARE_MODULE(timer);

static void
InitModules(JSContextRef ctx) {
    // Module initializers
    SetGlobalFunction(ctx, "__init_event",      __init_event);
    SetGlobalFunction(ctx, "__init_filesystem", __init_filesystem);
    SetGlobalFunction(ctx, "__init_font",       __init_font);
    SetGlobalFunction(ctx, "__init_graphics",   __init_graphics);
    SetGlobalFunction(ctx, "__init_image",      __init_image);
    SetGlobalFunction(ctx, "__init_keyboard",   __init_keyboard);
    SetGlobalFunction(ctx, "__init_timer",      __init_timer);
};

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

static JSValueRef
Eval(
    JSContextRef ctx,
    const std::string& script,
    const char* name)
{
    JSStringRef scriptStr = JSStringCreateWithUTF8CString(script.c_str());
    JSStringRef nameStr = JSStringCreateWithUTF8CString(name);
    JSValueRef exception = NULL;
    JSValueRef result =
        JSEvaluateScript(ctx, scriptStr, NULL, nameStr, 0, &exception);

    if (exception) {
        ReportException(ctx, exception);
    }

    return result;
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

//
// -----------------------------------------------------------------------
//

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

        if (i != 0) {
            printf(" ");
        }

        printf("%s", s);

        JSStringRelease(str);
        delete s;
    }

    printf("\n");

    return NULL;
}

#include <physfs.h>

static JSValueRef
Require(
    JSContextRef ctx,
    JSObjectRef function,
    JSObjectRef thisObject,
    size_t argCount,
    const JSValueRef args[],
    JSValueRef *exception)
{
    if (argCount == 1) {
        std::string filename = JSLGetString(ctx, args[0]);

        bool exists = PHYSFS_exists(filename.c_str());

        if (not exists) {
            // Try again with an extension.
            filename += ".js";
            exists = PHYSFS_exists(filename.c_str());
        }

        if (exists) {
            PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
            PHYSFS_sint64 length = PHYSFS_fileLength(file);

            char *buf = new char[length];
            int length_read = PHYSFS_read(file, buf, 1, length);
            PHYSFS_close(file);

            std::string script(buf, length_read);
            Eval(ctx, script, filename.c_str());

            delete buf;
        }
    }

    return JSValueMakeUndefined(ctx);
}

//
// -----------------------------------------------------------------------
//

static void
InitContext(JSContextRef ctx)
{
    SetGlobalFunction(ctx, "print", Print);
    SetGlobalFunction(ctx, "require", Require);

    InitModules(ctx);
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

    Eval(ctx, bootScript, "boot.js");

    JSGlobalContextRelease(ctx);

    return 0;
}
