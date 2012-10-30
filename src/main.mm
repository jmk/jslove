#import "jscimpl.h"
#import "v8impl.h"

#include <SDL.h>

int main(int argc, char** argv)
{
    // XXX: Need to make this portable.
    NSString* filePath = [[NSBundle mainBundle] pathForResource:@"boot" ofType:@"js"];
    NSString* bootScript = nil;
    if (filePath) {
        bootScript = [NSString stringWithContentsOfFile:filePath
                                               encoding:NSUTF8StringEncoding
                                                  error:nil];
    }

    if (not bootScript) {
        NSLog(@"Couldn't load boot.js");
        return 1;
    }

    std::string str([bootScript UTF8String]);

    JSCMain(str, argc, argv);
//    V8Main(str);

    return 0;
}
