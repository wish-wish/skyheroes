
#include "Feature.h"

Feature::Feature(Geometry* pGeo,const Style& style)
{
    m_pGeo = pGeo;
    m_Style = style;
}

Feature::Feature(Geometry* pGeo)
{
    m_pGeo = pGeo;
}

Feature::~Feature()
{
    if(m_pGeo)
        delete m_pGeo;
}

void Feature::SetStyle(const Style& style)
{
    m_Style = style;
}

Geometry* Feature::GetGeometry()
{
    return m_pGeo;
}

const Style& Feature::GetStyle()
{
    return m_Style;
}
