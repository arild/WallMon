 #import <Cocoa/Cocoa.h>

 static NSAutoreleasePool *pool = 0;

 void    updateAutoreleasePool(void) {
        [pool release];
        pool    = [[NSAutoreleasePool alloc] init];
 }

 void    freeAutoreleasePool(void) {
        [pool release];
        pool    = nil;
 }
