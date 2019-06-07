#include "fmod_bridge.h"
#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE

#import <AVFoundation/AVFoundation.h>

void FMODBridge_initIOSInterruptionHandler() {
    @autoreleasepool {
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification) {
            if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan) {
                FMODBridge_suspendMixer();
            } else {
                BOOL success = [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
                assert(success);
                FMODBridge_resumeMixer();
            }
        }];
    }
}
#endif

int FMODBridge_getBundleRoot(lua_State *L) {
    @autoreleasepool {
        NSString* path = [[NSBundle mainBundle] bundlePath];
        lua_pushstring(L, [path UTF8String]);
    }
    return 1;
}
