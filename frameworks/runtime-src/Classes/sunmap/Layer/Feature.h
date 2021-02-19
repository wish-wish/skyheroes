
#ifndef __sunmap__Feature__
#define __sunmap__Feature__

#include "Style.h"

#include <geos/geom/Geometry.h>

USING_NS_CC;

using namespace geos;
using namespace geos::geom;

class Feature
{
public:
    Feature(Geometry* pGeo,const Style& style);
    Feature(Geometry* pGeo);
    ~Feature();
    
    Geometry* GetGeometry();
    
    void SetStyle(const Style& style);
    const Style& GetStyle();
private:
    Geometry* m_pGeo;
    Style m_Style;
    
};

#endif /* defined(__sunmap__Feature__) */
