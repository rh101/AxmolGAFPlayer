#pragma once
#include "GAFCollections.h"
#include "GAFCachedTexture.h"

NS_GAF_BEGIN

class GAFSprite;
class GAFBlurFilterData;
class GAFGlowFilterData;
class GAFDropShadowFilterData;

class GAFFilterManager : public ax::Object
{
    typedef std::map<unsigned int, GAFCachedTexture> Cache_t;
    typedef std::pair<unsigned int, GAFCachedTexture> CachePair_t;
    
public:
    bool init();
    static GAFFilterManager* getInstance();
    ~GAFFilterManager() {}

    ax::Texture2D* applyFilter(ax::Sprite*, GAFFilterData*);

    void update(float dt);

    static void setCacheSize(size_t newSize);
    
private:
    GAFFilterManager() {}

    unsigned int hash(ax::Sprite*, GAFFilterData*);
    bool hasTexture(unsigned int);
    ax::Texture2D* renderFilteredTexture(ax::Sprite* sprite, GAFFilterData* filter);
    ax::Texture2D* renderFilteredTexture(ax::Sprite* sprite, GAFFilterData* filter, unsigned int hash);

    ax::Texture2D* renderBlurTexture(ax::Sprite* sprite, GAFBlurFilterData* filter);
    ax::Texture2D* renderGlowTexture(ax::Sprite* sprite, GAFGlowFilterData* filter);
    ax::Texture2D* renderShadowTexture(ax::Sprite* sprite, GAFDropShadowFilterData* filter);
    
    void insertTexture(ax::Texture2D*, unsigned int hash);
    
private:

    static Cache_t s_cache;
    static GAFFilterManager* s_instance;
    
    static size_t s_maxCacheSize;
};

NS_GAF_END
