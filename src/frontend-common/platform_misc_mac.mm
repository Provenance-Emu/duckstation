// SPDX-FileCopyrightText: 2019-2022 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: (GPL-3.0 OR CC-BY-NC-ND-4.0)

#include "platform_misc.h"
#include "common/log.h"
#include "common/string.h"

#import <TargetConditionals.h>
#if TARGET_OS_OSX
#include <IOKit/pwr_mgt/IOPMLib.h>
#import <AppKit/AppKit.h>
static IOPMAssertionID s_prevent_idle_assertion = kIOPMNullAssertionID;
#else
#import <UIKit/UIKit.h>
#endif
#include <cinttypes>
Log_SetChannel(FrontendCommon);



static bool SetScreensaverInhibitMacOS(bool inhibit)
{
#if TARGET_OS_OSX
  if (inhibit)
  {
    const CFStringRef reason = CFSTR("System Running");
    if (IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleDisplaySleep, kIOPMAssertionLevelOn, reason,
                                    &s_prevent_idle_assertion) != kIOReturnSuccess)
    {
      Log_ErrorPrintf("IOPMAssertionCreateWithName() failed");
      return false;
    }

    return true;
  }
  else
  {
    IOPMAssertionRelease(s_prevent_idle_assertion);
    s_prevent_idle_assertion = kIOPMNullAssertionID;
    return true;
  }
#endif
}

static bool s_screensaver_suspended;

void FrontendCommon::SuspendScreensaver()
{
  if (s_screensaver_suspended)

  if (!SetScreensaverInhibitMacOS(true))
  {
    Log_ErrorPrintf("Failed to suspend screensaver.");
    return;
  }

  s_screensaver_suspended = true;
}

void FrontendCommon::ResumeScreensaver()
{
  if (!s_screensaver_suspended)
    return;

  if (!SetScreensaverInhibitMacOS(false))
    Log_ErrorPrint("Failed to resume screensaver.");

  s_screensaver_suspended = false;
}

bool FrontendCommon::PlaySoundAsync(const char* path)
{
#if TARGET_OS_OSX
  NSString* nspath = [[NSString alloc] initWithUTF8String:path];
  NSSound* sound = [[NSSound alloc] initWithContentsOfFile:nspath byReference:YES];
  const bool result = [sound play];
  [sound release];
  [nspath release];
  return result;
#else
    return false;
#endif
}
