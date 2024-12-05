#pragma once

#include "GAFQuadCommand.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

typedef struct _gafBlendFuncSeparate
{
    // source blend function
    ax::backend::BlendFactor src;
    // destination blend function
    ax::backend::BlendFactor dst;
    // source alpha channel blend function
    ax::backend::BlendFactor srcAlpha;
    // destination alpha channel blend function
    ax::backend::BlendFactor dstAlpha;

} gafBlendFuncSeparate;

/// @class GAFSprite 
///	This is utility class used by GAF playback. Base class for all sprites
/// used by GAF.

class GAFSprite : public ax::Sprite
{
public:
    GAFSprite();

    bool initWithSpriteFrame(ax::SpriteFrame *spriteFrame, GAFRotation rotation);
    virtual bool initWithSpriteFrame(ax::SpriteFrame *spriteFrame) override;
    virtual bool initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated) override;
    void setTexture(ax::Texture2D *texture) override;

    virtual void setVertexRect(const ax::Rect& rect) override;
    virtual void setTextureRect(const ax::Rect& rect, bool rotated, const ax::Size& untrimmedSize) override;
    
    virtual void setTextureCoords(const ax::Rect& rect) override;
    virtual void setTextureCoords(const ax::Rect& rect, ax::V3F_C4B_T2F_Quad* outQuad) override;

    void setExternalTransform(const ax::AffineTransform& transform);
    const ax::AffineTransform& getExternalTransform() const;

    virtual const ax::Mat4& getNodeToParentTransform() const override;
    virtual ax::AffineTransform getNodeToParentAffineTransform() const override;
    virtual void draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags) override;
    
    void setAtlasScale(float scale);

    inline void setLocator(bool locator)
    {
        m_isLocator = locator;
    }

    inline float getAtlasScale() const { return m_atlasScale; }
    
protected:

    /**
    * Sets Uniforms for shader.
    *
    * Will be called before task with current program state is pushed to
    * the render queue. Must return hash of the currend shader program 
    * state including uniforms, textures, program using. If 0 is returned
    * renderrer will not attempt to batch this call.
    *
    * @return  hash value of the shader program state.
    */
    virtual uint32_t setUniforms();


    /* Members */
public:
    uint32_t objectIdRef;
protected:
    ax::AffineTransform    m_externalTransform;
    //ax::CustomCommand      m_customCommand;
    GAFQuadCommand              m_quadCommand;
private:

    /**
    * Quad is equal to _quad but transformed to view space
    */
    ax::V3F_C4B_T2F_Quad    m_quad;

    float                   m_atlasScale;
    bool                    m_isLocator;

    GAFRotation             m_rotation;
};

NS_GAF_END
