#include "GAFPrecompiled.h"
#include "GAFFilterManager.h"
#include "GAFFilterData.h"
#include "GAFShaderManager.h"

#include "xxhash/xxhash.h"

USING_NS_AX;
using namespace std;

NS_GAF_BEGIN

//#if GAF_ENABLE_NEW_UNIFORM_SETTER
//#define getUniformId(x) GAFShaderManager::getUniformLocation(x)
//#else
//#define getUniformId(x) GAFShaderManager::getUniformName(x)
//#endif

#define getUniformId(ps, x) (ps)->getUniformLocation(x)

static const int kGaussianKernelSize = 9;

GAFFilterManager::Cache_t GAFFilterManager::s_cache;
GAFFilterManager* GAFFilterManager::s_instance = nullptr;
size_t GAFFilterManager::s_maxCacheSize = 1024 * 1024 * 4;

/*static*/ void GAFFilterManager::setCacheSize(size_t newSize)
{
    s_maxCacheSize = newSize;
}

bool GAFFilterManager::init()
{
    //Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    return true;
}

void GAFFilterManager::update(float dt)
{
    (void)dt;
    //m_policy->update(dt);
}

Texture2D* GAFFilterManager::applyFilter(ax::Sprite* texture, GAFFilterData* filter)
{
    auto id = hash(texture, filter);
    if(hasTexture(id))
    {

        return *s_cache[id];
    }
    else
    {
        return renderFilteredTexture(texture, filter, id);
    }
}

unsigned int GAFFilterManager::hash(Sprite* sprite, GAFFilterData* filter)
{
    struct Hash
    {
        void* texture;
        Rect rect;
        GAFBlurFilterData blur;
        GAFGlowFilterData glow;
        GAFDropShadowFilterData shadow;
    };

    Hash hash;
    memset((void*)&hash, 0, sizeof(Hash));

    hash.texture = sprite->getTexture();  // getName();
    hash.rect = sprite->getTextureRect();

    if (filter->getType() == GAFFilterType::Blur)
    {
        hash.blur = *static_cast<GAFBlurFilterData*>(filter);
    }
    else if (filter->getType() == GAFFilterType::Glow)
    {
        hash.glow = *static_cast<GAFGlowFilterData*>(filter);
    }
    else if (filter->getType() == GAFFilterType::DropShadow)
    {
        hash.shadow = *static_cast<GAFDropShadowFilterData*>(filter);
    }
    
    return XXH32(&hash, sizeof(Hash), 0);
}

bool GAFFilterManager::hasTexture(unsigned int id)
{
    return s_cache.find(id) != s_cache.end();
}

Texture2D* GAFFilterManager::renderFilteredTexture(Sprite* sprite, GAFFilterData* filter)
{
    auto id = hash(sprite, filter);
    return renderFilteredTexture(sprite, filter, id);
}

ax::Texture2D* GAFFilterManager::renderGlowTexture(ax::Sprite* sprite, GAFGlowFilterData* filter)
{
    auto program = GAFShaderManager::getProgram(GAFShaderManager::EPrograms::Glow);

    const float blurRadiusX = (filter->blurSize.width / 4.f);
    const float blurRadiusY = (filter->blurSize.height / 4.f);

    Size textureSize = sprite->getTextureRect().size;
    Size rTextureSize = Size(textureSize.width + kGaussianKernelSize * blurRadiusX,
                             textureSize.height + kGaussianKernelSize * blurRadiusY);

    const int rTextureW = static_cast<int>(rTextureSize.width);
    const int rTextureH = static_cast<int>(rTextureSize.height);

    BlendFunc pureBlend = {backend::BlendFactor::ONE, backend::BlendFactor::ZERO};

    RenderTexture* outA = RenderTexture::create(rTextureSize.width, rTextureSize.height);
    // A
    // Draw desired sprite only
    {
        Sprite *s = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
        s->setFlippedY(sprite->isFlippedY());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setBlendFunc(pureBlend);

        outA->beginWithClear(0, 0, 0, 0);
        s->visit();
        outA->end();
    }

    RenderTexture* outB = RenderTexture::create(rTextureW, rTextureH);
    // B
    // Draw with blur over X coordinate
    {
        ax::Vec2 texelValue(blurRadiusX / rTextureSize.width, 0);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowTexelOffset));
        auto glowColorLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowColor));
        auto strengthLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::Strength));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));
        state->setUniform(glowColorLocation, &filter->color, sizeof(Color4F));
        auto strength = sqrt(filter->strength);
        state->setUniform(strengthLocation, &strength, sizeof(float));

        Sprite* s = Sprite::createWithTexture(outA->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outB->beginWithClear(0, 0, 0, 0);
        s->visit();
        outB->end();
    }

    RenderTexture* outC = RenderTexture::create(rTextureW, rTextureH);
    // C
    // Draw with blur over Y coordinate
    {
        ax::Vec2 texelValue(0, blurRadiusY / rTextureSize.height);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowTexelOffset));
        auto glowColorLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowColor));
        auto strengthLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::Strength));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));
        state->setUniform(glowColorLocation, &filter->color, sizeof(Color4F));
        auto strength = sqrt(filter->strength);
        state->setUniform(strengthLocation, &strength, sizeof(float));

        Sprite* s = Sprite::createWithTexture(outB->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outC->beginWithClear(0, 0, 0, 0);
        s->visit();
        outC->end();
    }
    {
        Sprite *s = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
        s->setFlippedY(sprite->isFlippedY());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        outC->begin();
        s->visit();
        outC->end();
    }
    Director::getInstance()->getRenderer()->render();
    return outC->getSprite()->getTexture();
}

ax::Texture2D* GAFFilterManager::renderBlurTexture(ax::Sprite* sprite, GAFBlurFilterData* filter)
{
    auto* program = GAFShaderManager::getProgram(GAFShaderManager::EPrograms::Blur);

    const float blurRadiusX = (filter->blurSize.width / 4.f);
    const float blurRadiusY = (filter->blurSize.height / 4.f);

    Size textureSize = sprite->getTextureRect().size;
    Size rTextureSize = Size(textureSize.width + kGaussianKernelSize * blurRadiusX,
                             textureSize.height + kGaussianKernelSize * blurRadiusY);

    const int rTextureW = static_cast<int>(rTextureSize.width);
    const int rTextureH = static_cast<int>(rTextureSize.height);

    BlendFunc pureBlend = {backend::BlendFactor::ONE, backend::BlendFactor::ZERO};

    RenderTexture* outA = RenderTexture::create(rTextureSize.width, rTextureSize.height);
    // A
    // Draw desired sprite only
    {
        Sprite *s = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
        s->setFlippedY(sprite->isFlippedY());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setBlendFunc(pureBlend);

        outA->beginWithClear(0, 0, 0, 0);
        s->visit();
        outA->end();
    }

    RenderTexture* outB = RenderTexture::create(rTextureW, rTextureH);
    // B
    // Draw with blur over X coordinate
    {
        ax::Vec2 texelValue(blurRadiusX / rTextureSize.width, 0);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::BlurTexelOffset));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));

        Sprite* s = Sprite::createWithTexture(outA->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outB->beginWithClear(0, 0, 0, 0);
        s->visit();
        outB->end();
    }

    RenderTexture* outC = RenderTexture::create(rTextureW, rTextureH);
    // C
    // Draw with blur over Y coordinate
    {
        ax::Vec2 texelValue(0, blurRadiusY / rTextureSize.height);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::BlurTexelOffset));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));

        Sprite* s = Sprite::createWithTexture(outB->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outC->beginWithClear(0, 0, 0, 0);
        s->visit();
        outC->end();
    }

    Director::getInstance()->getRenderer()->render();
    return outC->getSprite()->getTexture();
}

ax::Texture2D* GAFFilterManager::renderShadowTexture(ax::Sprite* sprite, GAFDropShadowFilterData* filter)
{
    auto* program = GAFShaderManager::getProgram(GAFShaderManager::EPrograms::Glow);

    const float blurRadiusX = (filter->blurSize.width / 4.f);
    const float blurRadiusY = (filter->blurSize.height / 4.f);

    Size textureSize = sprite->getTextureRect().size;
    Size rTextureSize = Size(textureSize.width + kGaussianKernelSize * blurRadiusX,
                             textureSize.height + kGaussianKernelSize * blurRadiusY);

    const int rTextureW = static_cast<int>(rTextureSize.width);
    const int rTextureH = static_cast<int>(rTextureSize.height);

    BlendFunc pureBlend = {backend::BlendFactor::ONE, backend::BlendFactor::ZERO};

    RenderTexture* outA = RenderTexture::create(rTextureSize.width, rTextureSize.height);
    // A
    // Draw desired sprite only
    {
        Sprite *s = Sprite::createWithTexture(sprite->getTexture(), sprite->getTextureRect());
        s->setFlippedY(sprite->isFlippedY());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setBlendFunc(pureBlend);

        outA->beginWithClear(0, 0, 0, 0);
        s->visit();
        outA->end();
    }

    RenderTexture* outB = RenderTexture::create(rTextureW, rTextureH);
    // B
    // Draw with blur over X coordinate
    {
        Vec2 texelValue(blurRadiusX / rTextureSize.width, 0);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowTexelOffset));
        auto glowColorLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowColor));
        auto strengthLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::Strength));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));
        state->setUniform(glowColorLocation, &filter->color, sizeof(Color4F));
        auto strength = 1.f;
        state->setUniform(strengthLocation, &strength, sizeof(float));

        Sprite* s = Sprite::createWithTexture(outA->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outB->beginWithClear(0, 0, 0, 0);
        s->visit();
        outB->end();
    }

    RenderTexture* outC = RenderTexture::create(rTextureW, rTextureH);
    // C
    // Draw with blur over Y coordinate
    {
        Vec2 texelValue(0, blurRadiusY / rTextureSize.height);

        auto* state = new (std::nothrow) ProgramState(program);
        auto texelValueLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowTexelOffset));
        auto glowColorLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::GlowColor));
        auto strengthLocation =
            getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::Strength));

        state->setUniform(texelValueLocation, &texelValue, sizeof(Vec2));
        state->setUniform(glowColorLocation, &filter->color, sizeof(Color4F));
        auto strength = 1.f;
        state->setUniform(strengthLocation, &strength, sizeof(float));

        Sprite* s = Sprite::createWithTexture(outB->getSprite()->getTexture());
        s->setPosition(rTextureSize.width / 2, rTextureSize.height / 2);
        s->setProgramState(state);
        s->setFlippedY(true);
        s->setBlendFunc(pureBlend);

        outC->beginWithClear(0, 0, 0, 0);
        s->visit();
        outC->end();
    }
    Director::getInstance()->getRenderer()->render();
    return outC->getSprite()->getTexture();
}

Texture2D* GAFFilterManager::renderFilteredTexture(Sprite* sprite, GAFFilterData* filter, unsigned int id)
{
    Texture2D* texture = nullptr;
    if (filter->getType() == GAFFilterType::Blur)
    {
        texture = renderBlurTexture(sprite, static_cast<GAFBlurFilterData*>(filter));
    }
    else if (filter->getType() == GAFFilterType::Glow)
    {
        texture = renderGlowTexture(sprite, static_cast<GAFGlowFilterData*>(filter));
    }
    else if (filter->getType() == GAFFilterType::DropShadow)
    {
        texture = renderShadowTexture(sprite, static_cast<GAFDropShadowFilterData*>(filter));
    }
    else
    {
        AXASSERT(false, "Errorr. No filter processor found!");
    }

    insertTexture(texture, id);

    return texture;
}

void GAFFilterManager::insertTexture(ax::Texture2D* texture, unsigned int id)
{
    AXASSERT(texture, "Failed to filter texture");
    s_cache[id] = texture;
    
    //m_policy->onInserted(id);
    
    if(s_cache.size() == 1)
    {
        return;
    }
    
    size_t totalSize = 0;
    for(auto cache : s_cache)
    {
        totalSize += cache.second.memoryUsed();
    }
    
    if(totalSize > s_maxCacheSize)
    {
        // Delete one texture
        auto later = s_cache.begin();
        time_t time = later->second.lastUsed();
        for(auto cache = ++s_cache.begin(), end = s_cache.end(); cache != end; ++cache)
        {
            if( cache->second.lastUsed() < time )
            {
                later = cache;
                time = cache->second.lastUsed();
            }
        }
        s_cache.erase(later);
    }
}

GAFFilterManager* GAFFilterManager::getInstance()
{
    if (!s_instance)
    {
        s_instance = new GAFFilterManager();
        s_instance->init();
    }
    return s_instance;
}

NS_GAF_END
