/******************************************************************************** 
 * create:cclin
 * date:2014-11-13
 * ver:
 * latest update:
 * function:动作族
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "CocosGUI.h"
#include "ActionExt.h"

USING_NS_CC;
using namespace std;
using namespace ui;

enum AroundType
{
    rtClockWise
    ,rtAntiClockWise
};


typedef Vector<Widget*> WidgetList;
typedef tuple<WidgetList,CallFuncN*,Size> WidgetTuple;

typedef queue<WidgetTuple> WidgetsActionTuple;
typedef vector<Vec2> PointList;
typedef vector<float> LengthList;

class ActionModule:public Singleton<ActionModule>,public Node
{
public:
	ActionModule();
	~ActionModule();
public:
	//弹出效果
	void OpenWidget();
	void HideWidget();

	//组动作
	bool ListItemsAction(ListView* list,CallFuncN* itemdone);
	bool WidgetsAction(Vector<Widget*>& widgets,CallFuncN* itemdone,Size s);
	
    Sequence* NewAroundAction(Rect rect,Vec2 startPos=Vec2(0,0),CallFuncN* onedone=nullptr,float livets=0,AroundType rt=rtClockWise);
    Sequence* NewAroundAction(Node* node,Vec2 startPos=Vec2(0,0),CallFuncN* onedone=nullptr,float livets=0,AroundType rt=rtClockWise);
    
    //time mode
    Sequence* NewAroundActionTime(PointList pl,Vec2 startPos=Vec2(0,0),CallFuncN* onedone=nullptr,float livets=0);
    //speed mode
    Sequence* NewAroundActionSpeed(PointList pl,Vec2 startPos=Vec2(0,0),CallFuncN* onedone=nullptr,float speed=1.0);
    
    Sequence* NewHintAnim(float livets=1.0,float scale=0.5,CallFuncN* onedone=nullptr,float scaleobj=1.0);
    Sequence* NewBornAnim(float livets=0.5,CallFuncN* onedone=nullptr,float scaleobj=1.0);
private:
	mutex m_queue_mtx;
	WidgetsActionTuple m_WidgetActionQueue;

	void doAction(float delta);
};