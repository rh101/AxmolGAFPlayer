#pragma once

NS_GAF_BEGIN

class GAFResourcesInfo
{
public:
    enum class ResourceId : uint16_t
    {
        Texture = 0,
        Font,
        Sound, // this one for future usage
    };

    ResourceId id;
};

class GAFResourcesInfoTexture : public GAFResourcesInfo
{
public:
    GAFResourcesInfoTexture() { id = ResourceId::Texture; }
    GAFResourcesInfoTexture(std::string _source, float _csf)
    {
        id = ResourceId::Texture;
        source = std::move(_source);
        csf = _csf;
    }

    bool operator==(const GAFResourcesInfoTexture &other) const
    {
        return source == other.source && fabs(csf - other.csf) < std::numeric_limits<float>::epsilon();
    }

    std::string source;
    float csf{};
};

class GAFResourcesInfoFont : public GAFResourcesInfo
{
public:
    GAFResourcesInfoFont() { id = ResourceId::Font; }
    GAFResourcesInfoFont(std::string _name)
    {
        id = ResourceId::Font;
        name = std::move(_name);
    }

    bool operator==(const GAFResourcesInfoFont& other) const { return name == other.name; }

    std::string name;
};

NS_GAF_END