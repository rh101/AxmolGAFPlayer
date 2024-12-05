#pragma once

#include "GAFTextureAtlas.h"
#include "GAFDelegates.h"

NS_GAF_BEGIN

class GAFAssetTextureManager : public ax::Object
{
public:
	GAFAssetTextureManager();
	~GAFAssetTextureManager();

	void					appendInfoFromTextureAtlas(GAFTextureAtlas* atlas);
	void					loadImages(const std::string& dir, GAFTextureLoadDelegate_t delegate, ax::ZipFile* bundle = nullptr);
	ax::Texture2D*		getTextureById(uint32_t id);
    bool                    swapTexture(uint32_t id, ax::Texture2D* texture);
    
	uint32_t				getMemoryConsumptionStat() const;

private:
	typedef std::map<size_t, ax::Image*> ImagesMap_t;
	typedef std::map<size_t, ax::Texture2D*> TexturesMap_t;

	bool isAtlasInfoPresent(const GAFTextureAtlas::AtlasInfo &ai);

	GAFTextureAtlas::AtlasInfos_t m_atlasInfos;

	ImagesMap_t m_images;
	TexturesMap_t m_textures;

	uint32_t m_memoryConsumption;
};

NS_GAF_END