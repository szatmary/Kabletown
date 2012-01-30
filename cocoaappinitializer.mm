#include "cocoaappinitializer.h"
#include <Cocoa/Cocoa.h>

class CocoaAppInitializer::Private {
    public:
    NSAutoreleasePool* autoReleasePool;
};

CocoaAppInitializer::CocoaAppInitializer() {
    d = new CocoaAppInitializer::Private();
    d->autoReleasePool = [[NSAutoreleasePool alloc] init];
}

CocoaAppInitializer::~CocoaAppInitializer() {
    [d->autoReleasePool release];
    delete d;
}
