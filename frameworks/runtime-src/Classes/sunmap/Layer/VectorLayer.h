
#ifndef __sunmap__VectorLayer__
#define __sunmap__VectorLayer__

#include "cocos2d.h"
#include "MapControlLayer.h"
#include "Feature.h"

USING_NS_CC;

class VectorLayer : public Layer
{
public:
	VectorLayer(MapControl* pControl);
    ~VectorLayer();
    
    void AddFeatureLayer(Feature* pFeature);
    void AddFeatureLayers(Feature* pFeature,int length);
    
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
    
    virtual void update(float delta);
    
	void onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
	void onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
	void onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
  
private:
    void drawFeatures();
    
private:
    std::vector<Feature*> m_Features;
    MapControl* m_pControl;
    
    Vec2 m_tBeginPos;
    
    Vec2* m_points;
    int m_nPntNum;
    Vec2 m_pnt;

	DrawNode* m_draw;
    
};


#endif /* defined(__sunmap__VectorLayer__) */
