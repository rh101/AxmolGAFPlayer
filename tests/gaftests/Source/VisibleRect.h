#pragma once

#include "axmol.h"

class VisibleRect
{
public:
    static ax::Rect getVisibleRect();

    static ax::Vec2 left();
    static ax::Vec2 right();
    static ax::Vec2 top();
    static ax::Vec2 bottom();
    static ax::Vec2 center();
    static ax::Vec2 leftTop();
    static ax::Vec2 rightTop();
    static ax::Vec2 leftBottom();
    static ax::Vec2 rightBottom();
private:
    static void lazyInit();
    static ax::Rect s_visibleRect;
};
