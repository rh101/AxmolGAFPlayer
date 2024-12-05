#include "GAFPrecompiled.h"
#include "GAFSprite.h"
#include "GAFCollections.h"

#include "math/TransformUtils.h"
#include "xxhash/xxhash.h"

USING_NS_AX;

#if AX_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__A__) ( (int)(__A__))
#endif

NS_GAF_BEGIN

GAFSprite::GAFSprite()
: objectIdRef(IDNONE)
, m_isLocator(false)
, m_atlasScale(1.0f)
, m_externalTransform(AffineTransform::IDENTITY)
, m_rotation(GAFRotation::NONE)
{
}

bool GAFSprite::initWithSpriteFrame(ax::SpriteFrame *spriteFrame, GAFRotation rotation)
{
    m_rotation = rotation;
    return initWithSpriteFrame(spriteFrame);
}

bool GAFSprite::initWithSpriteFrame(ax::SpriteFrame *spriteFrame)
{
    AXASSERT(spriteFrame != nullptr, "");

    bool bRet = ax::Sprite::initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect());
    setSpriteFrame(spriteFrame);

    return bRet;
}

bool GAFSprite::initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated)
{
    if (ax::Sprite::initWithTexture(pTexture, rect, rotated))
    {
        setProgramStateByProgramId(backend::ProgramType::POSITION_TEXTURE_COLOR);
        return true;
    }
    else
    {
        return false;
    }
}

void GAFSprite::setTexture(ax::Texture2D *texture)
{
    Sprite::setTexture(texture);
    //AXASSERT(!_batchNode || (texture && texture == _batchNode->getTexture()),
    //         "CCSprite: Batched sprites should use the same texture as the batchnode");
    //// accept texture==nil as argument
    //AXASSERT(!texture || dynamic_cast<ax::Texture2D*>(texture), "setTexture expects a ax::Texture2D. Invalid argument");

    //if (!_batchNode && _texture != texture)
    //{
    //    AX_SAFE_RETAIN(texture);
    //    AX_SAFE_RELEASE(_texture);
    //    _texture = texture;
    //    updateBlendFunc();
    //}
}

void GAFSprite::setVertexRect(const ax::Rect& rect)
{
    Sprite::setVertexRect(rect);
    if (m_rotation != GAFRotation::NONE)
    {
        std::swap(_rect.size.width, _rect.size.height);
    }
}

void GAFSprite::setTextureRect(const ax::Rect& rect, bool rotated, const ax::Size& untrimmedSize)
{
    ax::Size rotatedSize = untrimmedSize;
    if (m_rotation != GAFRotation::NONE)
    {
        rotated = true;
        rotatedSize = ax::Size(rotatedSize.height, rotatedSize.width);
    }
    ax::Sprite::setTextureRect(rect, rotated, rotatedSize);
}

void GAFSprite::setTextureCoords(const ax::Rect& rect)
{
    ax::Sprite::setTextureCoords(rect, &_quad);
}

void GAFSprite::setTextureCoords(const ax::Rect& rect, ax::V3F_C4B_T2F_Quad* outQuad)
{
    Texture2D* tex =
        (_renderMode == RenderMode::QUAD_BATCHNODE) ? _textureAtlas->getTexture() : _texture;

    if (tex == nullptr)
        return;

    const auto rectInPixels = AX_RECT_POINTS_TO_PIXELS(rect);

    const float atlasWidth  = (float)tex->getPixelsWide();
    const float atlasHeight = (float)tex->getPixelsHigh();

    float rw = rectInPixels.size.width;
    float rh = rectInPixels.size.height;
    
    if (_rectRotated)
        std::swap(rw, rh);
    
    float left = rectInPixels.origin.x / atlasWidth;
    float right = (rectInPixels.origin.x + rw) / atlasWidth;
    float top = rectInPixels.origin.y / atlasHeight;
    float bottom = (rectInPixels.origin.y + rh) / atlasHeight;
    
    switch (m_rotation)
    {
        case gaf::GAFRotation::CCW_90:
        {
            if (_flippedX)
            {
                std::swap(top, bottom);
            }
            
            if (_flippedY)
            {
                std::swap(left, right);
            }
            
            outQuad->bl.texCoords.u = right;
            outQuad->bl.texCoords.v = bottom;
            outQuad->br.texCoords.u = right;
            outQuad->br.texCoords.v = top;
            outQuad->tl.texCoords.u = left;
            outQuad->tl.texCoords.v = bottom;
            outQuad->tr.texCoords.u = left;
            outQuad->tr.texCoords.v = top;
        }
            break;
            
        case gaf::GAFRotation::CW_90:
        {
            if (_flippedX)
            {
                std::swap(top, bottom);
            }
            
            if (_flippedY)
            {
                std::swap(left, right);
            }
            
            outQuad->bl.texCoords.u = left;
            outQuad->bl.texCoords.v = top;
            outQuad->br.texCoords.u = left;
            outQuad->br.texCoords.v = bottom;
            outQuad->tl.texCoords.u = right;
            outQuad->tl.texCoords.v = top;
            outQuad->tr.texCoords.u = right;
            outQuad->tr.texCoords.v = bottom;
        }
            break;
            
        case gaf::GAFRotation::NONE:
        default:
        {
            if (_flippedX)
            {
                std::swap(left, right);
            }
            
            if (_flippedY)
            {
                std::swap(top, bottom);
            }
            
            outQuad->bl.texCoords.u = left;
            outQuad->bl.texCoords.v = bottom;
            outQuad->br.texCoords.u = right;
            outQuad->br.texCoords.v = bottom;
            outQuad->tl.texCoords.u = left;
            outQuad->tl.texCoords.v = top;
            outQuad->tr.texCoords.u = right;
            outQuad->tr.texCoords.v = top;
        }
            break;
    }
}

void GAFSprite::setExternalTransform(const ax::AffineTransform& transform)
{
    if (!ax::AffineTransformEqualToTransform(getExternalTransform(), transform))
    {
        m_externalTransform = transform;
        _transformUpdated = true;
        _transformDirty = true;
        _inverseDirty = true;
    }
}

const ax::AffineTransform& GAFSprite::getExternalTransform() const
{
    return m_externalTransform;
}

const ax::Mat4& GAFSprite::getNodeToParentTransform() const
{
    if (_transformDirty)
    {
        if (m_atlasScale != 1.f)
        {
            ax::AffineTransform transform = ax::AffineTransformScale(getExternalTransform(), m_atlasScale, m_atlasScale);
            ax::CGAffineToGL(ax::AffineTransformTranslate(transform, -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        }
        else
        {
            ax::CGAffineToGL(ax::AffineTransformTranslate(getExternalTransform(), -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        }
    }

    if (_additionalTransform)
    {
        // This is needed to support both Node::setNodeToParentTransform() and Node::setAdditionalTransform()
        // at the same time. The scenario is this:
        // at some point setNodeToParentTransform() is called.
        // and later setAdditionalTransform() is called every time. And since _transform
        // is being overwritten everyframe, _additionalTransform[1] is used to have a copy
        // of the last "_transform without _additionalTransform"
        if (_transformDirty)
            _additionalTransform[1] = _transform;

        if (_transformUpdated)
            _transform = _additionalTransform[1] * _additionalTransform[0];
    }

    _transformDirty = _additionalTransformDirty = false;

    return _transform;
}

ax::AffineTransform GAFSprite::getNodeToParentAffineTransform() const
{
    ax::AffineTransform transform;
    if (_transformDirty)
    {
        transform = getExternalTransform();
        if (m_atlasScale != 1.0f)
        {
            transform = ax::AffineTransformScale(transform, m_atlasScale, m_atlasScale);
        }

        ax::CGAffineToGL(ax::AffineTransformTranslate(transform, -_anchorPointInPoints.x, -_anchorPointInPoints.y), _transform.m);
        _transformDirty = false;
    }
    ax::GLToCGAffine(_transform.m, &transform);

    return transform;
}

void GAFSprite::draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags)
{
    (void)flags;
    if (m_isLocator)
    {
        return;
    }

    uint32_t id = setUniforms();

    m_quad = _quad;

    transform.transformPoint(&m_quad.tl.vertices);
    transform.transformPoint(&m_quad.tr.vertices);
    transform.transformPoint(&m_quad.bl.vertices);
    transform.transformPoint(&m_quad.br.vertices);

    auto& pipelineDescriptor        = m_quadCommand.getPipelineDescriptor();
    if (pipelineDescriptor.programState != _programState)
    {
        pipelineDescriptor.programState = _programState;
    }

    auto& pipelineQuad = m_quadCommand.getPipelineDescriptor();

    const auto& projectionMat = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    pipelineQuad.programState->setUniform(_mvpMatrixLocation, projectionMat.m, sizeof(projectionMat.m));
    pipelineQuad.programState->setTexture(_texture->getBackendTexture());

    m_quadCommand.init(_globalZOrder, _texture, _blendFunc, &m_quad, 1, Mat4::IDENTITY, id);
    renderer->addCommand(&m_quadCommand);
}

void GAFSprite::setAtlasScale(float scale)
{
    if (m_atlasScale != scale)
    {
        m_atlasScale = scale;
        _transformDirty = true;
        _inverseDirty = true;
    }
}

uint32_t GAFSprite::setUniforms()
{
    uint32_t materialID = Renderer::MATERIAL_ID_DO_NOT_BATCH;
    if (_programState)
    {
        struct
        {
            void* texture;
            void* prog;
            backend::BlendFactor src;
            backend::BlendFactor dst;
        } hashMe = {};

        // NOTE: Initialize hashMe struct to make the value of padding bytes be filled with zero.
        // It's important since XXH32 below will also consider the padding bytes which probably
        // are set to random values by different compilers.

        hashMe.texture = _texture;
        hashMe.src     = getBlendFunc().src;
        hashMe.dst     = getBlendFunc().dst;
        hashMe.prog    = _programState->getProgram();
        materialID     = XXH32((const void*)&hashMe, sizeof(hashMe), 0);
    }
    return materialID;
}

NS_GAF_END
