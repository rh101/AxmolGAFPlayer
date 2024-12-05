#include "GAFPrecompiled.h"
#include "GAFQuadCommand.h"

USING_NS_AX;

NS_GAF_BEGIN

GAFQuadCommand::GAFQuadCommand()
{}

GAFQuadCommand::~GAFQuadCommand()
{}

void GAFQuadCommand::init(float globalOrder,
                          Texture2D* texture,
                          BlendFunc blendType,
                          V3F_C4B_T2F_Quad* quad,
                          ssize_t quadCount,
                          const Mat4& mv,
                          uint32_t materialId)
{
    QuadCommand::init(globalOrder, texture,  blendType, quad, quadCount, mv, 0);

    if (_materialID != materialId) 
    {
        _materialID = materialId;
        if (_materialID != ax::Renderer::MATERIAL_ID_DO_NOT_BATCH)
        {
            setSkipBatching(false);
        }
    }
}

NS_GAF_END
