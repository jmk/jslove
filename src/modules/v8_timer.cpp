#include "v8_timer.h"

#include <timer/sdl/timer.h>

using namespace v8;
using love::timer::sdl::Timer;

static Timer* _t = NULL;

class WrapModule
{
protected:
    static Handle<Value> _Call(const Arguments& args, double x) {
        if (args.Length() == 0) {
            return Number::New(x);
        } else {
            return Undefined();
        }
    }
};

class WrapTimer : WrapModule
{
public:
    static Handle<Value> step(const Arguments& args) {
        _t->step();
        return Undefined();
    }

    static Handle<Value> sleep(const Arguments& args) {
        if (args.Length() == 1) {
            if (args[0]->IsNumber()) {
                double x = args[0]->NumberValue();
                _t->sleep(x);
            }
        }
        return Undefined();
    }

    static Handle<Value> getDelta(const Arguments& args) {
        return _Call(args, _t->getDelta());
    }

    static Handle<Value> getFPS(const Arguments& args) {
        return _Call(args, _t->getFPS());
    }

    static Handle<Value> getTime(const Arguments& args) {
        return _Call(args, _t->getTime());
    }

    static Handle<Value> getMicroTime(const Arguments& args) {
        return _Call(args, _t->getMicroTime());
    }
};

Handle<Value> init_timer(const Arguments& args)
{
#if 0
    typedef love::timer::sdl::Timer Timer;
    Wrap<Timer>("Timer")
        .construct<int>()
        .construct<int, int>()
        .function("getY", &Point::getY)
        .function("setY", &Point::setY)
        .function("printPoint", &Point::printPoint)
        .function("printPointConstRef", &Point::printPointConstRef)
        .property("x", &Point::getX, &Point::setX)
        ;
#endif

    printf("INIT TIMER!\n");

    _t = new Timer();

    Handle<FunctionTemplate> timerTemplate = FunctionTemplate::New();
    Handle<Template> proto = timerTemplate->PrototypeTemplate();

    proto->Set("step",
               FunctionTemplate::New(WrapTimer::step));
    proto->Set("sleep",
               FunctionTemplate::New(WrapTimer::sleep));
    proto->Set("getDelta",
               FunctionTemplate::New(WrapTimer::getDelta));
    proto->Set("getFPS",
               FunctionTemplate::New(WrapTimer::getFPS));
    proto->Set("getTime",
               FunctionTemplate::New(WrapTimer::getTime));
    proto->Set("getMicroTime",
               FunctionTemplate::New(WrapTimer::getMicroTime));

    return timerTemplate->GetFunction()->NewInstance();
}
