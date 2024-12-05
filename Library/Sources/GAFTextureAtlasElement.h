#pragma once

#include "GAFCollections.h"

NS_GAF_BEGIN

class GAFTextureAtlasElement
{
public:
    std::string name;
    ax::Vec2      pivotPoint;
    ax::Rect      bounds;
    
    unsigned int       atlasIdx;
    unsigned int       elementAtlasIdx;

    GAFRotation        rotation;

    GAFTextureAtlasElement();  

    void setScale(float s);
    float getScale() const { return scale; }
    void setScaleX(float s);
    float getScaleX() const { return scaleX; }
    void setScaleY(float s);
    float getScaleY() const { return scaleY; }

private:
    float              scale;
    float              scaleX;
    float              scaleY;
}; // GAFTextureAtlasElement

NS_GAF_END