#include "GAFPrecompiled.h"
#include "GAFTextField.h"

NS_GAF_BEGIN

GAFTextField::GAFTextField()
{
    m_charType = GAFCharacterType::TextField;
}

GAFTextField::~GAFTextField()
{

}

// TODO: Factory
void GAFTextField::initWithTextData(GAFTextData const* data)
{
    m_label = ax::Label::createWithSystemFont(data->m_text, data->m_textFormat.m_font, data->m_textFormat.m_size);
    
    m_label->setDimensions(data->m_width, data->m_height);
    m_label->setTextColor(ax::Color4B(data->m_textFormat.m_color));
    m_label->setHorizontalAlignment(data->m_textFormat.getTextAlignForCocos());

    ax::Point anchor = ax::Point(data->m_pivot.x / data->m_width, 1 - data->m_pivot.y / data->m_height);
    m_label->setAnchorPoint(anchor);

    addChild(m_label);
}

NS_GAF_END