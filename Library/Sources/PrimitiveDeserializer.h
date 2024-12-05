#pragma once

NS_GAF_BEGIN

class GAFStream;

class PrimitiveDeserializer
{
    struct GAFReadColor
    {
        unsigned char b, g, r, a;
    };
public:
    static void deserialize(GAFStream* in, ax::Vec2* out);
    static void deserialize(GAFStream* in, ax::Rect* out);
    static void deserialize(GAFStream* in, ax::AffineTransform* out);
    static void deserializeSize(GAFStream* in, ax::Size* out);
    static void deserialize(GAFStream* in, ax::Color4B* out);
    static void translateColor(ax::Color4F& out, unsigned int in);
};

NS_GAF_END