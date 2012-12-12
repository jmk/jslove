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

bool JSLGetBoolean(
    JSContextRef ctx,
    JSValueRef value,
    bool defaultValue)
{
    if (JSValueIsBoolean(ctx, value)) {
        return JSValueToBoolean(ctx, value);
    } else {
        printf("ERROR: invalid boolean\n");  // XXX raise exception
        return defaultValue;
    }
}

double JSLGetNumber(
    JSContextRef ctx,
    JSValueRef value,
    double defaultValue)
{
    if (JSValueIsNumber(ctx, value)) {
        return JSValueToNumber(ctx, value, NULL);
    } else {
        printf("ERROR: invalid number\n");  // XXX raise exception
        return defaultValue;
    }
}

std::string JSLGetString(
    JSContextRef ctx,
    JSValueRef value,
    const std::string& defaultValue)
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
        printf("ERROR: invalid string\n");  // XXX raise exception
        return defaultValue;
    }
}

JSValueRef
JSLMakeStringValue(JSContextRef ctx, const std::string& str)
{
    JSStringRef jsStr = JSStringCreateWithUTF8CString(str.c_str());
    JSValueRef result = JSValueMakeString(ctx, jsStr);
    JSStringRelease(jsStr);
    return result;
}

//
// JSLObjectProxy
//

_JSLObjectProxy::_JSLObjectProxy(void* data_, JSLTypeId typeId_) :
    typeId(typeId_),
    data(data_)
{
    // Do nothing.
}

//
// Class/instance utilities
//

//bool JSLObjectIsA(JSObjectRef obj, JSLTypeId t)
//{
//    void* p = JSObjectGetPrivate(obj);
//    _JSLObjectProxy* x = reinterpret_cast<_JSLObjectProxy*>(p);
//    return p and (x->typeId & t).any();
//}

JSObjectRef JSLCreateObject(JSContextRef ctx)
{
    // TODO: Finalize callback should free private data.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    return JSObjectMake(ctx, cls, /* data */ NULL);
}

//JSObjectRef JSLCreateObject(JSContextRef ctx, void* data, JSLTypeId typeId)
//{
//    JSObjectRef obj = JSLCreateObject(ctx);
//    if (obj and data) {
//        _JSLObjectProxy* proxy = new _JSLObjectProxy(data, typeId);
//        bool success = JSObjectSetPrivate(obj, proxy);
//
//        if (not success) {
//            printf("ERROR: Failed to set private data\n");
//        }
//    }
//
//    return obj;
//}
