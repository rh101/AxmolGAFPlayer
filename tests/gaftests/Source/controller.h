#pragma once

#include "axmol.h"

USING_NS_AX;

class TestController : public Layer
{
public:
    TestController();
    ~TestController();

    void menuCallback(ax::Object * sender);
    void closeCallback(ax::Object * sender);

    bool onTouchBegan(Touch* touches, Event  *event);
    void onTouchMoved(Touch* touches, Event  *event);

    void onMouseScroll(Event *event);
    void addConsoleAutoTest();
    void autorun();
    void startAutoRun();
    void runAllTests(int fd);
    ssize_t readline(int fd, char* ptr, size_t maxlen);
private:
    Vec2 _beginPos;
    Menu* _itemMenu;
    bool _exitThread;
};
