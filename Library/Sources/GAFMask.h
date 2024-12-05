#pragma once

#include "GAFObject.h"

NS_GAF_BEGIN

class GAFMask : public GAFObject
{
public:
    GAFMask();
    ~GAFMask() override;
    virtual bool initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated) override;

    virtual void draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags) override;
    virtual void visit(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags) override;
};

NS_GAF_END
