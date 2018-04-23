#include "fmod_bridge.hpp"

#if TARGET_OS_IPHONE

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

void FMODBridge::initIOSInterruptionHandler() {
  [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification) {
      if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan) {
          FMODBridge::suspendMixer();
      } else {
          BOOL success = [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
          assert(success);
          FMODBridge::resumeMixer();
      }
  }];
}
#endif