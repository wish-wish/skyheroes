#ifndef __COCOS2D_MarkerLayer_H__
#define __COCOS2D_MarkerLayer_H__

#include "cocos2d.h"
#include "MapControlLayer.h"
#include "SpriteWithHue.h"

USING_NS_CC;

struct Marker;
struct Func;

typedef std::function<bool(Marker*, Func* func, Sprite*, int, Touch *, Event *)> MarkCallBack;

struct Func
{
	std::string m_name;
	std::string m_pic;
	float m_scale;
	int m_state;
	Vec2 m_pos;
	int m_tag;
	int m_select;
	std::string m_text;
	int m_ref;
	Sprite* m_sp;
	bool m_showtitle;
	std::string m_title_pic;
	std::string m_title;
};

struct Marker
{
	std::string m_name;
	std::string m_pic;
	unsigned int m_uid;
	int m_tag;
	Vec2 m_pos;//lat,lon
	Vec2 m_pt;//display
	Vec2 m_pt_m;//click temp, for temp operate sprite
	int m_state;
	int m_select;
	int m_type;//0,±êÖ¾£¬1,²Ù×÷
	float m_scale;
	bool m_showoper;
	int m_transition;
	bool m_showtitle;
	std::string m_cn_name;
	std::string m_en_name;
	int m_ct_type;
	std::string m_sections_name;
	std::string m_title_pic;
	std::string m_title;
	std::vector<Func*> m_funcs;
	MarkCallBack m_callback;
	Sprite* m_sp;
	std::string m_msg;
	Node* m_parent;
};

typedef std::vector<Marker*> TMarkList;

class MarkerLayer : public Layer
{
public:
	MarkerLayer(MapControl* pControl);
	void onDelay(float delta);
	bool onClickOperate(Marker* mark, Func* func, Sprite* ahue, int atype, Touch * atouch, Event * aevent);
	~MarkerLayer();
	void onClick(Ref* sender);
	bool onClickSprite(Sprite* ahue, int atype, Touch * atouch, Event * aevent);
	void refreshMarker();
	void addFlagSprite(Marker* mark);
	void addOperateSprite(Marker* mark);
	void showOperateSprite(Marker* mark);
	void closeOperateSprite(Marker* mark);
	void showTitle(Marker* mark);
	void showTitle(Func* mark);
	std::string randomPic();
	void addOperates(std::string strImage, Vec2 pnt, int tag, bool show, std::string params, std::vector<Func> operates, MarkCallBack cb);
	void AddMarker(std::string strImage, Vec2 pnt, std::string param);
	void DelMarker(Marker* mark);
	void str_split(std::string cnt, std::string delim, std::vector<std::string> *citys);
	Vec2 dispCenter(Vec2 center,float radius,Size size,Size wsize);

    //virtual void update(float delta);
    
	void onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent);
	void onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent);
	void onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent);

	void setCurrZoom(int zoom);
	int getCurrZoom() { return m_currZoom; };

	Vec2 getLatLon(Vec2 pos);

	void OneTree();
	void setSelType(int atype);
private:
    TMarkList m_marks;
	TMarkList m_selects;
	MapControl* m_pControl;
	int m_currZoom;
	Vec2 m_tBeginPos;
	void onCustomEvent(EventCustom* cevent);
	bool isInSelect(Marker* mark);
	static int m_maxid;
	Marker* getMarker(int tag);
	bool m_iszooming;
	int m_sel_type;
	DrawNode* m_draw;
	void clearMarks();
	void selectMarks(Vec2 start,Vec2 end);
	//Vec2 m_temppos;
};



#endif