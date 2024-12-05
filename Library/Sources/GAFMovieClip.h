#pragma once

#include "GAFObject.h"

NS_GAF_BEGIN

class GAFColorMatrixFilterData;
class GAFGlowFilterData;
class GAFBlurFilterData;

class GAFMovieClip : public GAFObject
{
private:
    void _setBlendingFunc();
    void handleStencilProgram();

protected:
    ax::Vec4                    m_colorTransformMult;
    ax::Vec4                    m_colorTransformOffsets;
    ax::Mat4                    m_colorMatrixIdentity1;
    ax::Vec4                    m_colorMatrixIdentity2;
    GAFColorMatrixFilterData*   m_colorMatrixFilterData;
    GAFGlowFilterData*          m_glowFilterData;
    GAFBlurFilterData*          m_blurFilterData;
    ax::Texture2D *             m_initialTexture;
    ax::Rect                    m_initialTextureRect;
    ax::ProgramState*           m_programBase;
    ax::ProgramState*           m_programNoCtx;
    mutable bool                m_ctxDirty;
    bool                        m_isStencil;

    void updateTextureWithEffects();
    virtual uint32_t setUniforms() override;

public:

    GAFMovieClip();
    virtual ~GAFMovieClip() override;

    virtual bool initWithTexture(ax::Texture2D *pTexture, const ax::Rect& rect, bool rotated) override;

    void setColorTransform(const float * mults, const float * offsets);
    void setColorTransform(const float * colorTransform);

    void setColorMarixFilterData(GAFColorMatrixFilterData* data);
    void setGlowFilterData(GAFGlowFilterData* data);
    void setBlurFilterData(GAFBlurFilterData* data);

    ax::Texture2D*    getInitialTexture() const;
    const ax::Rect&   getInitialTextureRect() const;

    bool            hasCtx();
    void            updateCtx();
    //void            setGLProgram(ax::Program *glProgram); // For monitoring external program changes

    bool setProgramState(ax::ProgramState* programState, bool ownPS) override;

    virtual void draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags) override;
};

NS_GAF_END