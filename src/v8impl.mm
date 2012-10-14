#include "v8impl.h"

#import <v8.h>

#import <assert.h>

using namespace v8;

extern Handle<Value> init_timer(const Arguments& args);

#if 0
Handle<Value> Require(const Arguments& args) {
    for (int i = 0; i < args.Length(); i++) {
        String::Utf8Value str(args[i]);
        
        // load_file loads the file with this name into a string,
        // I imagine you can write a function to do this :)
        std::string js_file = load_file(*str);
        
        if(js_file.length() > 0) {
            Handle<String> source = String::New(js_file.c_str());
            Handle<Script> script = Script::Compile(source);
            return script->Run();
        }
    }
    return Undefined();
}

Handle<ObjectTemplate> global = ObjectTemplate::New();

global->Set(String::New("require"), FunctionTemplate::New(Require));
#endif

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

static Handle<Value>
Print(const Arguments& args)
{
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        HandleScope handle_scope;
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        String::Utf8Value str(args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
    return Undefined();
}



// Creates a new execution environment containing the built-in
// functions.
static Persistent<Context>
CreateShellContext() {
    // Create a template for the global object.
    Handle<ObjectTemplate> global = ObjectTemplate::New();
    
    global->Set(String::New("print"), FunctionTemplate::New(Print));
    
    global->Set(String::New("__love_init_timer"), FunctionTemplate::New(init_timer));
    
    return Context::New(NULL, global);
}

void
ReportException(TryCatch* try_catch)
{
    HandleScope handle_scope;
    String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    Handle<Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        printf("%s\n", exception_string);
    } else {
        // Print (filename):(line number): (message).
        
        String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        printf("%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        printf("%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++) {
            printf(" ");
        }
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++) {
            printf("^");
        }
        printf("\n");
        String::Utf8Value stack_trace(try_catch->StackTrace());
        if (stack_trace.length() > 0) {
            const char* stack_trace_string = ToCString(stack_trace);
            printf("%s\n", stack_trace_string);
        }
    }
}

// Executes a string within the current v8 context.
static bool
ExecuteString(Handle<String> source,
              Handle<Value> name,
              bool print_result,
              bool report_exceptions)
{
    HandleScope handle_scope;
    TryCatch try_catch;
    Handle<Script> script = Script::Compile(source, name);
    if (script.IsEmpty()) {
        // Print errors that happened during compilation.
        if (report_exceptions)
            ReportException(&try_catch);
        return false;
    } else {
        Handle<Value> result = script->Run();
        if (result.IsEmpty()) {
            assert(try_catch.HasCaught());
            // Print errors that happened during execution.
            if (report_exceptions)
                ReportException(&try_catch);
            return false;
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                // If all went well and the result wasn't undefined then print
                // the returned value.
                String::Utf8Value str(result);
                const char* cstr = ToCString(str);
                printf("%s\n", cstr);
            }
            return true;
        }
    }
}


void
Run(Handle<Context> context, const std::string& str)
{
    // Enter the execution environment before evaluating any code.
    Context::Scope contextScope(context);
    
    HandleScope handleScope;
    
    //    std::string str = "print(\"y helo thar\")";
    //    std::string str =
    //        "function obj() {}; var asdf = new obj(); asdf.doShit = function(x, y) {\n"
    //        "    print([1, 2, 3], 'bla bla', Math.random(), x, y);\n"
    //        "};";

    Local<String> name(String::New("(foo)"));
    ExecuteString(String::New(str.c_str()),
                  name,
                  /* print_result */ false,
                  /* report_exceptions */ true);
    
#if 0
    Handle<String> moduleName = String::New("love");
    Handle<Value> moduleVal = context->Global()->Get(moduleName);
    
    if (not moduleVal->IsObject()) {
        NSLog(@"module not found!");
        return;
    }
    
    Handle<Object> module = Handle<Object>::Cast(moduleVal);
    
    Handle<String> funcName = String::New("run");
    Handle<Value> funcVal = module->Get(funcName);
    
    // If there is no valid function, bail out
    if (not funcVal->IsFunction()) {
        NSLog(@"function not found!");
        return;
    }
    
    // It is a function; cast it to a Function
    Handle<Function> func = Handle<Function>::Cast(funcVal);
    
    // Set up an exception handler before calling the function
    TryCatch tryCatch;
    
    // Invoke the process function, giving the global object as 'this'
    // and one argument, the request.
    Local<String> foo(String::New("jajajaja"));
    Local<Number> bar(Number::New(999));
    
    const int argc = 2;
    Handle<Value> argv[argc] = { foo, bar };
    Handle<Value> result = func->Call(context->Global(), argc, argv);
    
    if (result.IsEmpty()) {
        ReportException(&tryCatch);
    }
#endif
}

// Placeholder entry point.
int V8Main(const std::string& str)
{
    
    //    V8::SetFlagsFromCommandLine(&argc, argv, true);
    //    run_shell = (argc == 1);
    
    {
        HandleScope handle_scope;
        Persistent<Context> context = CreateShellContext();
        
        if (context.IsEmpty()) {
            printf("Error creating context\n");
            return 1;
        }
        
        context->Enter();
        Run(context, str);
        context->Exit();
        
        context.Dispose();
    }
    V8::Dispose();
    return 0;
}


