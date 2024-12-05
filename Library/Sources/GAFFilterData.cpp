#include "GAFPrecompiled.h"
#include "GAFFilterData.h"

#include "GAFMovieClip.h"
#include "GAFFilterManager.h"

NS_GAF_BEGIN

GAFBlurFilterData::GAFBlurFilterData():
GAFFilterData(GAFFilterType::Blur)
{

}

void GAFBlurFilterData::apply(GAFMovieClip* subObject)
{
    subObject->setBlurFilterData(this);
}

const float GAFColorMatrixFilterData::Grayscale::matrix[16] = { 
    0.299f, 0.299f, 0.299f, 0,
    0.587f, 0.587f, 0.587f, 0,
    0.114f, 0.114f, 0.114f, 0,
    0, 0, 0, 1 
};
const float GAFColorMatrixFilterData::Grayscale::matrix2[4] = { 0, 0, 0, 0 };

GAFColorMatrixFilterData::GAFColorMatrixFilterData() :
	GAFFilterData(GAFFilterType::ColorMatrix), matrix{}, matrix2{}
{
}

void GAFColorMatrixFilterData::setMatrix(const float m[16])
{
    std::copy(m, m + 16, matrix);
}

void GAFColorMatrixFilterData::setMatrix2(const float m[4])
{
    std::copy(m, m + 4, matrix2);
}

void GAFColorMatrixFilterData::apply(GAFMovieClip* subObject)
{
    subObject->setColorMarixFilterData(this);
}

GAFGlowFilterData::GAFGlowFilterData():
	GAFFilterData(GAFFilterType::Glow), strength(0), innerGlow(false), knockout(false)
{
}

void GAFGlowFilterData::apply(GAFMovieClip* subObject)
{
    subObject->setGlowFilterData(this);
}

GAFDropShadowFilterData::GAFDropShadowFilterData():
	GAFFilterData(GAFFilterType::DropShadow), angle(0), distance(0), strength(0), innerShadow(false), knockout(false)
{
}

const int kShadowObjectTag = 0xFAD0;

void GAFDropShadowFilterData::apply(GAFMovieClip* subObject)
{
    ax::Texture2D* texture = subObject->getInitialTexture();
    const ax::Rect& texRect = subObject->getInitialTextureRect();
    ax::Texture2D* shadow = GAFFilterManager::getInstance()->applyFilter(ax::Sprite::createWithTexture(texture, texRect), this);
    ax::Sprite* shadowSprite = ax::Sprite::createWithTexture(shadow);
    reset(subObject);

    shadowSprite->setTag(kShadowObjectTag);
    shadowSprite->setOpacity(static_cast<uint8_t>(ax::clampf(strength, 0.0, 1.0) * 255));

    const float anglerad = ((float)M_PI / 180.f) * angle;
    //ax::Size shadowTextureSize = shadowSprite->getContentSize();
    ax::Vec2 offset = ax::Vec2(cos(anglerad) * distance, -sin(anglerad) * distance);
    shadowSprite->setPosition(ax::Vec2(texRect.size / 2) + offset);
    subObject->addChild(shadowSprite, -1);
}

void GAFDropShadowFilterData::reset(GAFMovieClip* subObject)
{
    ax::Node* prevShadowObject = subObject->getChildByTag(kShadowObjectTag);

    if (prevShadowObject)
    {
        subObject->removeChild(prevShadowObject, true);
    }
}

NS_GAF_END