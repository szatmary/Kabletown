#ifndef COCOAAPPINITIALIZER_H
#define COCOAAPPINITIALIZER_H

class CocoaAppInitializer {
public:
    CocoaAppInitializer();
    ~CocoaAppInitializer();

private:
    class Private;
    Private* d;
};

#endif // COCOAAPPINITIALIZER_H
