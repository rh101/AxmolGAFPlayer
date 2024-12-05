#include "GAFPrecompiled.h"

#include "GAFAssetTextureManager.h"

#if AX_ENABLE_CACHE_TEXTURE_DATA
#include "renderer/TextureCache.h"
#endif

#define ENABLE_GAF_MANUAL_PREMULTIPLY 0

NS_GAF_BEGIN

GAFAssetTextureManager::GAFAssetTextureManager():
m_memoryConsumption(0)
{

}

GAFAssetTextureManager::~GAFAssetTextureManager()
{
    GAF_SAFE_RELEASE_MAP(ImagesMap_t, m_images);
    GAF_SAFE_RELEASE_MAP(TexturesMap_t, m_textures);    
}

void GAFAssetTextureManager::appendInfoFromTextureAtlas(GAFTextureAtlas* atlas)
{
	GAFTextureAtlas::AtlasInfos_t atlasInfos = atlas->getAtlasInfos();
	GAFTextureAtlas::AtlasInfos_t::const_iterator i = atlasInfos.begin(), e = atlasInfos.end();
	for (; i != e; i++)
	{
		if (!isAtlasInfoPresent(*i))
		{
			m_atlasInfos.push_back(*i);
		}
	}
}

bool GAFAssetTextureManager::isAtlasInfoPresent(const GAFTextureAtlas::AtlasInfo &ai)
{
	GAFTextureAtlas::AtlasInfos_t::const_iterator i = m_atlasInfos.begin(), e = m_atlasInfos.end();
	for (; i != e; i++)
	{
		if (i->id == ai.id)
		{
			return true;
		}
	}
	return false;
}

void GAFAssetTextureManager::loadImages(const std::string& dir, GAFTextureLoadDelegate_t delegate, ax::ZipFile* bundle)
{
	std::stable_sort(m_atlasInfos.begin(), m_atlasInfos.end(), GAFTextureAtlas::compareAtlasesById);

	m_images.clear(); // check
	
	if (!m_atlasInfos.empty())
	{
		for (unsigned int i = 0; i < m_atlasInfos.size(); ++i)
		{
			GAFTextureAtlas::AtlasInfo& info = m_atlasInfos[i];

			std::string source;

			for (unsigned int j = 0; j < info.m_sources.size(); ++j)
			{
				GAFTextureAtlas::AtlasInfo::Source& aiSource = info.m_sources[j];
				if (1.f == aiSource.csf)
				{
					source = aiSource.source;
				}

				if (aiSource.csf == ax::Director::getInstance()->getContentScaleFactor())
				{
					source = aiSource.source;
					break;
				}
			}

			ax::Image* image = new ax::Image();
			std::string path = ax::FileUtils::getInstance()->fullPathFromRelativeFile(source, dir);

			if (delegate)
			{
                path = delegate(path);
			}

			if (!bundle)
			{
				image->initWithImageFile(path);
			}
			else
			{
                ax::Data data;
                ax::ResizableBufferAdapter buffer(&data);

				if (!bundle->getFileData(path, &buffer))
					return;

				if (buffer.size() == 0)
					return;

				image->initWithImageData(data.data(), data.getSize());
			}

			m_memoryConsumption += image->getDataLen();

#if ENABLE_GAF_MANUAL_PREMULTIPLY
			if (!image->hasPremultipliedAlpha() && image->hasAlpha())
			{
				//Premultiply
				unsigned char* begin = image->getData();
				unsigned int width = image->getWidth();
				unsigned int height = image->getHeight();
				int Bpp = image->getBitPerPixel() / 8;
				unsigned char* end = begin + width * height * Bpp;
				for (auto data = begin; data < end; data += Bpp)
				{
					unsigned int* wordData = (unsigned int*)(data);
					*wordData = AX_RGB_PREMULTIPLY_ALPHA(data[0], data[1], data[2], data[3]);
				}
			}
#endif
			m_images[info.id] = image;
		}
	}
}

ax::Texture2D* GAFAssetTextureManager::getTextureById(uint32_t id)
{
	TexturesMap_t::const_iterator txIt = m_textures.find(id);
	if (txIt != m_textures.end())
	{
		return txIt->second;
	}
    
    // check if still not created
    ImagesMap_t::const_iterator imagesIt = m_images.find(id);
    if (imagesIt != m_images.end())
    {
        ax::Texture2D * texture = new ax::Texture2D();
        texture->initWithImage(imagesIt->second);
        m_textures[id] = texture;
#if AX_ENABLE_CACHE_TEXTURE_DATA
        ax::VolatileTextureMgr::addImage(texture, imagesIt->second);
#endif
        imagesIt->second->release();
        m_images.erase(imagesIt);
        return texture;
    }

    return nullptr;
}

bool GAFAssetTextureManager::swapTexture(uint32_t id, ax::Texture2D *texture)
{
    TexturesMap_t::const_iterator txIt = m_textures.find(id);
    if (txIt != m_textures.end())
    {
        txIt->second->release();
        m_textures.erase(txIt);
    }
    
    ImagesMap_t::const_iterator imagesIt = m_images.find(id);
    if (imagesIt != m_images.end())
    {
        imagesIt->second->release();
        m_images.erase(imagesIt);
    }
    
#if AX_ENABLE_CACHE_TEXTURE_DATA
    // NOTE: this should not work with ax::VolatileTextureMgr
    //ax::VolatileTextureMgr::addImage(texture, imagesIt->second);
#endif
    
    texture->retain();
    m_textures[id] = texture;
    return true;
}

uint32_t GAFAssetTextureManager::getMemoryConsumptionStat() const
{
	return m_memoryConsumption;
}

NS_GAF_END
