#include "fmod_bridge.h"
#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE

#import <AVFoundation/AVFoundation.h>

#define AUDIO_RESUME_RETRY_COUNT 20
#define AUDIO_RESUME_RETRY_INTERVAL 0.2

static NSTimer * resumeTimer = nil;

// In case it doesn't work from the first try, retry activating the audio session
// a few times. For example, after dismissing a phone call, [AVAudioSession setActive: error:]
// errors out from the first try, but works on the second
static void resumeAudioSession(int retries) {
    @autoreleasepool {
        NSError * error;
        BOOL success = [[AVAudioSession sharedInstance] setActive:TRUE error: &error];
        if (success) {
            FMODBridge_resumeMixer();
            return;
        }

        if (error) {
            LOGW("Cannot activate audio session: %s", [[error localizedDescription] UTF8String]);
        } else {
            LOGW("Cannot activate audio session and no error was issued");
        }

        if (retries <= 0) {
            LOGE("Cannot activate audio session. Not retrying anymore.");
            return;
        }

        resumeTimer = [NSTimer
            scheduledTimerWithTimeInterval: AUDIO_RESUME_RETRY_INTERVAL
            repeats: NO
            block: ^(NSTimer *timer) {
                resumeTimer = nil;
                resumeAudioSession(retries - 1);
            }
        ];
    }
}

void FMODBridge_initIOSInterruptionHandler() {
    @autoreleasepool {
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification) {
            NSUInteger typeValue = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue];

            if (typeValue == AVAudioSessionInterruptionTypeBegan) {
                if (resumeTimer) {
                    [resumeTimer invalidate];
                    resumeTimer = nil;
                }
                FMODBridge_suspendMixer();

            } else if (typeValue == AVAudioSessionInterruptionTypeEnded) {
                NSUInteger optionsValue = [[notification.userInfo valueForKey:AVAudioSessionInterruptionOptionKey] intValue];
                if (optionsValue & AVAudioSessionInterruptionOptionShouldResume) {
                    resumeAudioSession(AUDIO_RESUME_RETRY_COUNT);
                }
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
