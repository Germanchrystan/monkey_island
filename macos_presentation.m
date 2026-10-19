#import "macos_presentation.h"

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>

void enableMacOSKioskPresentation(void)
{
  @autoreleasepool
  {
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp setPresentationOptions:(NSApplicationPresentationHideDock |
                                   NSApplicationPresentationHideMenuBar)];
  }
}

void disableMacOSKioskPresentation(void)
{
  @autoreleasepool
  {
    [NSApp setPresentationOptions:NSApplicationPresentationDefault];
  }
}

#else

void enableMacOSKioskPresentation(void) {}
void disableMacOSKioskPresentation(void) {}

#endif
