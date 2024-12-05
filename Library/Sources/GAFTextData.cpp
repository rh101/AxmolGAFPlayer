#include "GAFPrecompiled.h"
#include "GAFTextData.h"

NS_GAF_BEGIN

ax::TextHAlignment GAFTextData::TextFormat::getTextAlignForCocos() const
{
    switch (m_align)
    {
    case TextAlign::Left:
        return ax::TextHAlignment::LEFT;
    case TextAlign::Right:
        return ax::TextHAlignment::RIGHT;
    case TextAlign::Center:
        return ax::TextHAlignment::CENTER;
    default:
        AXASSERT(false, "Not Implemented");
    }
    return ax::TextHAlignment::LEFT;
}

NS_GAF_END