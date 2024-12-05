#include "GAFPrecompiled.h"
#include "GAFMovieClip.h"

#include "GAFShaderManager.h"

#include "GAFSubobjectState.h"

#include "GAFFilterData.h"
#include "GAFFilterManager.h"

#include "xxhash/xxhash.h"

USING_NS_AX;

NS_GAF_BEGIN

struct GAFMovieClipHash
{
    Program*   program;
    Texture2D* texture;
    BlendFunc blend;
    Vec4    a;
    Vec4    b;
    float   c;
    Mat4    d;
    Vec4    e;        
};
    

GAFMovieClip::GAFMovieClip():
m_initialTexture(nullptr),
m_colorMatrixFilterData(nullptr),
m_glowFilterData(nullptr),
m_blurFilterData(nullptr),
m_programBase(nullptr),
m_programNoCtx(nullptr),
m_ctxDirty(false),
m_isStencil(false)
{
    m_objectType = GAFObjectType::MovieClip;
    m_charType = GAFCharacterType::Texture;
}

GAFMovieClip::~GAFMovieClip()
{
    AX_SAFE_RELEASE(m_initialTexture);
    if (!m_isStencil)
        _programState = nullptr; // Should be treated here as weak pointer
    AX_SAFE_RELEASE(m_programBase);
    AX_SAFE_RELEASE(m_programNoCtx);
}

bool GAFMovieClip::initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated)
{
    if (GAFSprite::initWithTexture(pTexture, rect, rotated))
    {
        m_initialTexture = pTexture;
        m_initialTexture->retain();
        m_initialTextureRect = rect;
        m_colorTransformMult = ax::Vec4::ONE;
        m_colorTransformOffsets = ax::Vec4::ZERO;
        _setBlendingFunc();

        m_programBase = new ProgramState(GAFShaderManager::getProgram(GAFShaderManager::EPrograms::Alpha));

#if CHECK_CTX_IDENTITY
        auto p = ProgramManager::getInstance()->getBuiltinProgram(ax::backend::ProgramType::POSITION_TEXTURE_COLOR);
        AXASSERT(p, "Error! Program SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP not found.");
        m_programNoCtx = new backend::ProgramState(p);
#endif
#if CHECK_CTX_IDENTITY
        _programState = m_programNoCtx;
#else
        _programState = m_programBase;
#endif
        return true;
    }
    else
    {
        return false;
    }
}

//void GAFMovieClip::setGLProgram(Program *glProgram)
//{
//    if (_programState == nullptr || (_programState && _glProgramState->getGLProgram() != glProgram))
//    {
//        auto alphaTest = ProgramManager::getInstance()->getBuiltinProgram(
//            ax::backend::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
//
//        if (glProgram == alphaTest)
//        {
//            // This node is set as stencil
//            handleStencilProgram();
//        }
//        Node::setGLProgram(glProgram);
//    }
//}

bool GAFMovieClip::setProgramState(ax::ProgramState* programState, bool ownPS)
{
    if (_programState == nullptr || (_programState && _programState->getProgram() != programState->getProgram()))
    {
        auto alphaTest = ProgramManager::getInstance()->getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);

        if (programState->getProgram() == alphaTest)
        {
            // This node is set as stencil
            handleStencilProgram();
        }
    }
    return GAFSprite::setProgramState(programState, ownPS);
}

void GAFMovieClip::handleStencilProgram()
{
    _programState = nullptr; // Weaken pointer;
    m_isStencil = true; // Object can not stop being stencil
}

void GAFMovieClip::updateTextureWithEffects()
{
    if (!m_blurFilterData && !m_glowFilterData)
    {
        setTexture(m_initialTexture);
        setTextureRect(m_initialTextureRect, false, m_initialTextureRect.size);
        setFlippedY(false);
    }
    else
    {
        ax::Texture2D * resultTex = nullptr;

        if (m_blurFilterData)
        {
            resultTex = GAFFilterManager::getInstance()->applyFilter(Sprite::createWithTexture(m_initialTexture, m_initialTextureRect), m_blurFilterData);
        }
        else if (m_glowFilterData)
        {
            resultTex = GAFFilterManager::getInstance()->applyFilter(Sprite::createWithTexture(m_initialTexture, m_initialTextureRect), m_glowFilterData);
        }

        if (resultTex)
        {
            setTexture(resultTex);
            setFlippedY(true);
            ax::Rect texureRect = ax::Rect(0, 0, resultTex->getContentSize().width, resultTex->getContentSize().height);
            setTextureRect(texureRect, false, texureRect.size);
        }
    }
}

uint32_t GAFMovieClip::setUniforms()
{
    if (m_isStencil)
    {
        return GAFSprite::setUniforms();
    }

#define getUniformId(ps, x) (ps)->getUniformLocation(x)

#if CHECK_CTX_IDENTITY
    const bool ctx = hasCtx();
#else
    const bool ctx = false;
#endif

    auto* state = getProgramState();

    GAFMovieClipHash hash;
    memset(&hash, 0, sizeof(GAFMovieClipHash));

    hash.program = state->getProgram();
    hash.texture = _texture;
    hash.blend = _blendFunc;

    if (!ctx)
    {
        Color4F color(m_colorTransformMult.x, m_colorTransformMult.y, m_colorTransformMult.z, m_colorTransformMult.w);
        Node::setColor(Color3B(color));
        Node::setOpacity(static_cast<GLubyte>(color.a * 255.0f));
    }
    else
    {
        {
            hash.a = m_colorTransformMult;
            hash.b = m_colorTransformOffsets;
            const auto colorTransformMultLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorTransformMult));
            const auto colorTransformOffsetLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorTransformOffset));

            state->setUniform(colorTransformMultLocation, &m_colorTransformMult, sizeof(ax::Vec4));
            state->setUniform(colorTransformOffsetLocation, &m_colorTransformOffsets, sizeof(ax::Vec4));
        }

        if (!m_colorMatrixFilterData)
        {
            hash.d = ax::Mat4::IDENTITY;
            hash.e = ax::Vec4::ZERO;
            const auto colorMatrixBodyLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorMatrixBody));
            const auto colorMatrixAppendixLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorMatrixAppendix));

        	state->setUniform(colorMatrixBodyLocation, &ax::Mat4::IDENTITY, sizeof(ax::Mat4));
            state->setUniform(colorMatrixAppendixLocation, &ax::Vec4::ZERO, sizeof(ax::Vec4));
        }
        else
        {
            hash.d = Mat4(m_colorMatrixFilterData->matrix);
            hash.e = Vec4(m_colorMatrixFilterData->matrix2);
            const auto colorMatrixBodyLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorMatrixBody));
            const auto colorMatrixAppendixLocation = getUniformId(state, GAFShaderManager::getUniformName(GAFShaderManager::EUniforms::ColorMatrixAppendix));

            state->setUniform(colorMatrixBodyLocation, &m_colorMatrixFilterData->matrix, sizeof(m_colorMatrixFilterData->matrix));
            state->setUniform(colorMatrixAppendixLocation, &m_colorMatrixFilterData->matrix2, sizeof(m_colorMatrixFilterData->matrix2));
        }
    }
    return XXH32((void*)&hash, sizeof(GAFMovieClipHash), 0);
}
void GAFMovieClip::setColorTransform(const GLfloat * mults, const GLfloat * offsets)
{
    if (m_isStencil)
    {
        return;
    }
    m_colorTransformMult = Vec4(mults);
    m_colorTransformOffsets = Vec4(offsets);
    _setBlendingFunc();
    m_ctxDirty = true;
}

void GAFMovieClip::setColorTransform(const GLfloat * colorTransform)
{
    if (m_isStencil)
    {
        return;
    }
    m_colorTransformMult = Vec4(colorTransform);
    m_colorTransformOffsets = Vec4(&colorTransform[4]);
    _setBlendingFunc();
    m_ctxDirty = true;
}

void GAFMovieClip::_setBlendingFunc()
{
    setBlendFunc(ax::BlendFunc::ALPHA_PREMULTIPLIED);
}

void GAFMovieClip::setColorMarixFilterData(GAFColorMatrixFilterData* data)
{
    m_colorMatrixFilterData = data;
}

void GAFMovieClip::setGlowFilterData(GAFGlowFilterData* data)
{
    if (!m_isStencil && (m_glowFilterData != data))
    {
        m_glowFilterData = data;
        updateTextureWithEffects();
    }
}

void GAFMovieClip::setBlurFilterData(GAFBlurFilterData* data)
{
    if (!m_isStencil && (m_blurFilterData != data))
    {
        m_blurFilterData = data;
        updateTextureWithEffects();
    }
}

ax::Texture2D* GAFMovieClip::getInitialTexture() const
{
    return m_initialTexture;
}

const ax::Rect& GAFMovieClip::getInitialTextureRect() const
{
    return m_initialTextureRect;
}

void GAFMovieClip::updateCtx()
{
    if (m_isStencil)
    {
        return;
    }
    m_ctxDirty = false;
    if (!m_colorTransformOffsets.isZero() || m_colorMatrixFilterData || m_isManualColor)
    {
        _programState = m_programBase;
    }
    else
    {
        _programState = m_programNoCtx;
    }
}

bool GAFMovieClip::hasCtx()
{
    if (m_ctxDirty)
        updateCtx();

    return _programState == m_programBase;
}

void GAFMovieClip::draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags)
{
    GAFSprite::draw(renderer, transform, flags);
}

NS_GAF_END
