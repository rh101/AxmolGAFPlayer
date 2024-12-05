#include "GAFPrecompiled.h"
#include "GAFMask.h"
#include "base/Macros.h"

#define USE_LAYERED_STENCIL 0

NS_GAF_BEGIN

GAFMask::GAFMask()
{
    m_charType = GAFCharacterType::Texture;
    m_objectType = GAFObjectType::Mask;
}

GAFMask::~GAFMask()
{
}

bool GAFMask::initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated)
{
    if (!Sprite::initWithTexture(pTexture, ax::Rect(0, 0, rect.size.width, rect.size.height), rotated))
    {
        return false;
    }

    // Fixed just another bug in cocos
    auto* program = ax::ProgramManager::getInstance()->getBuiltinProgram(ax::ProgramType::POSITION_TEXTURE_COLOR);
    auto* programState = new (std::nothrow) ax::ProgramState(program);
    setProgramState(programState);

    return true;
}

void GAFMask::draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags)
{
    return Sprite::draw(renderer, transform, flags);
}

void GAFMask::visit(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags)
{
    // Clipping node works well only with cocos sprite for now.
    return Sprite::visit(renderer, transform, flags);
}

NS_GAF_END
