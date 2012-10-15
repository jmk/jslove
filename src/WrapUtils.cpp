#include "WrapUtils.h"

void JSLAddFunction(
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

bool JSLGetBoolean(JSContextRef ctx, JSValueRef value)
{
    if (JSValueIsBoolean(ctx, value)) {
        return JSValueToBoolean(ctx, value);
    } else {
        return false;
    }
}

double JSLGetNumber(JSContextRef ctx, JSValueRef value)
{
    if (JSValueIsNumber(ctx, value)) {
        return JSValueToNumber(ctx, value, NULL);
    } else {
        return 0.0;
    }
}

std::string JSLGetString(JSContextRef ctx, JSValueRef value)
{
    if (JSValueIsString(ctx, value)) {
        JSStringRef str = JSValueToStringCopy(ctx, value, NULL);

        size_t size = JSStringGetMaximumUTF8CStringSize(str);
        char* s = new char[size];
        JSStringGetUTF8CString(str, s, size);
        JSStringRelease(str);

        std::string result(s);
        delete s;

        return result;
    } else {
        return std::string();
    }
}
