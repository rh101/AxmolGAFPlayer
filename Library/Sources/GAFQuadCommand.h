#pragma once

NS_GAF_BEGIN

/** Command used to render one or more Quads */
class GAFQuadCommand : public ax::QuadCommand
{
public:
    GAFQuadCommand();
    ~GAFQuadCommand() override;

    void init(float globalOrder,
              ax::Texture2D* texture,
              ax::BlendFunc blendType,
              ax::V3F_C4B_T2F_Quad* quads,
              ssize_t quadCount,
              const ax::Mat4& mv,
              uint32_t materialId);
};

NS_GAF_END