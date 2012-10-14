#import "JSLAppDelegate.h"

#import "jscimpl.h"
#import "v8impl.h"

@implementation JSLAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
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
        return;
    }
    
    std::string str([bootScript UTF8String]);

    JSCMain(str);
//    V8Main(str);
}

@end
