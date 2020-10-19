#define SLOW   50   // steps/second
#define FAST 2000   // steps/second

class PlatformControl {
  public:
    void doParkBlocking() {}
    void doUnparkBlocking() {}
    void movePark(int speed) {}
    void moveTop(int speed) {}
    void moveDown(int speed) {}
    void moveUp(int speed) {}
    void stop() {}
    bool moving() {}
    long position() {}
    void zeroPosition() {}
};
