#include "fmod_bridge.h"

#if TARGET_OS_IPHONE

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

void FMODBridge_initIOSInterruptionHandler() {
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
#endif