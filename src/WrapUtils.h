#ifndef __jslove__wraputils__
#define __jslove__wraputils__

#include <JavaScriptCore/JavaScript.h>

#include <string>

//
// General utilities
//

void JSLAddFunction(
    JSContextRef ctx, JSObjectRef obj, const char* name,
    JSObjectCallAsFunctionCallback callback);

bool JSLGetBoolean(
    JSContextRef, JSValueRef, bool defaultValue = false);

double JSLGetNumber(
    JSContextRef, JSValueRef, double defaultValue = 0.0);

std::string JSLGetString(
    JSContextRef, JSValueRef,
    const std::string& defaultValue = std::string());

JSValueRef JSLMakeStringValue(JSContextRef, const std::string&);

#define WRAP_FUNCTION(name)       \
    static JSValueRef name (      \
        JSContextRef ctx,         \
        JSObjectRef function,     \
        JSObjectRef self,         \
        size_t argCount,          \
        const JSValueRef args[],  \
        JSValueRef *exception)    \

#define WRAP_MODULE(name)                \
    JSValueRef __wrap_module_ ## name (  \
        JSContextRef ctx)

//
// Object/type utilities
//

// XXX: It would be nice to avoid this direct dependency on love2d.
#include <common/types.h>
typedef love::bits JSLTypeId;

struct _JSLObjectProxy
{
    _JSLObjectProxy(void* data, JSLTypeId typeId);

    void* const data;
    const JSLTypeId typeId;

#if 0
    // True if the JavaScript runtime owns the underlying object.
    const bool owned;
#endif
};

template <typename T>
struct _JSLTypeTraits
{
    static const JSLTypeId typeId;
    static const std::string name;
};

#define DECLARE_TYPE(T, typeId_)        \
    template <>                         \
    struct _JSLTypeTraits<T> {          \
        static const JSLTypeId typeId;  \
        static const std::string name;  \
    };                                  \
    const JSLTypeId _JSLTypeTraits<T>::typeId = typeId_;  \
    const std::string _JSLTypeTraits<T>::name = #T;

// XXX: TEMP
bool JSLObjectIsA(JSObjectRef obj, love::bits t);

template <typename T>
bool JSLObjectIsA(JSObjectRef obj)
{
    void* p = JSObjectGetPrivate(obj);
    _JSLObjectProxy* x = reinterpret_cast<_JSLObjectProxy*>(p);
    return p and (x->typeId & _JSLTypeTraits<T>::typeId).any();
}

template <typename T>
T* JSLExtractObject(JSObjectRef obj)
{
    void* p = JSObjectGetPrivate(obj);

    if (not p) {
        return NULL;
    }

    _JSLObjectProxy* x = reinterpret_cast<_JSLObjectProxy*>(p);
    if (not (x->typeId & _JSLTypeTraits<T>::typeId).any()) {
        printf("ERROR: Couldn't extract object of type: %s\n",
               _JSLTypeTraits<T>::name.c_str());
        return NULL;
    }

    return reinterpret_cast<T*>(x->data);
};

template <typename T>
T* JSLExtractObject(JSContextRef ctx, JSValueRef val)
{
    JSObjectRef obj = JSValueToObject(ctx, val, NULL);
    return obj ? JSLExtractObject<T>(obj) : NULL;
}

//
// Class/instance utilities
//

#define WRAP_CLASS(name)               \
    JSValueRef __wrap_class ## name (  \
        JSContextRef ctx)

JSObjectRef JSLCreateObject(JSContextRef ctx);
JSObjectRef JSLCreateObject(JSContextRef ctx, void* data, JSLTypeId typeId);

#endif
