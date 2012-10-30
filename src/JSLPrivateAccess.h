#ifndef __jslove__JSLPrivateAccess__
#define __jslove__JSLPrivateAccess__

#include "WrapUtils.h"

#include <JavaScriptCore/JavaScript.h>

#include <common/Variant.h>
#include <event/sdl/Event.h>

#include <string>

// Helper to access private members.
struct JSLPrivateAccess
{
    struct Message {
        static int nargs(love::event::Message* m) {
            return m->nargs;
        }

        static std::string name(love::event::Message* m) {
            return m->name;
        }

        static love::Variant* arg(love::event::Message* m, int n) {
            return m->args[n];
        }
    };

    struct Variant {
        static JSValueRef toValue(love::Variant* v, JSContextRef ctx) {
            switch (v->type) {
                case love::Variant::BOOLEAN:
                    return JSValueMakeBoolean(ctx, v->data.boolean);

                case love::Variant::CHARACTER:
                    return JSLMakeStringValue(
                        ctx, std::string(&(v->data.character)));

                case love::Variant::NUMBER:
                    return JSValueMakeNumber(ctx, v->data.number);

                case love::Variant::STRING:
                    return JSLMakeStringValue(
                        ctx, std::string(v->data.string.str,
                                         v->data.string.len));

                default:
                    return JSValueMakeUndefined(ctx);
            }
        }
    };
};

#endif
