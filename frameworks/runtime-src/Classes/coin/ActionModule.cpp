/******************************************************************************** 
 * create:cclin
 * date:2014-11-13
 * ver:
 * latest update:
 * function:
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "ActionModule.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

const unsigned int Tag_TAG=99999;

ActionModule::ActionModule()
{
	this->retain();
}

ActionModule::~ActionModule()
{
	this->release();
}

void ActionModule::OpenWidget()
{

}

void ActionModule::HideWidget()
{

}

bool ActionModule::ListItemsAction(ListView* list,CallFuncN* itemdone)
{
	bool ret=true;
	if(list==NULL)
		return false;
	Vector<Widget*>& children = list->getItems();	
	return this->WidgetsAction(children,itemdone,list->getSize());
}

bool ActionModule::WidgetsAction(Vector<Widget*>& widgets,CallFuncN* itemdone,Size s)
{
	bool ret=true;
	if(widgets.size() == 0){
		ret = false;
	}
	m_queue_mtx.lock();
	m_WidgetActionQueue.push(make_tuple(widgets,itemdone,s));
	m_queue_mtx.unlock();
	if(itemdone!=nullptr)
		itemdone->retain();
	if(!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(ActionModule::doAction),this))
		Director::getInstance()->getScheduler()->schedule(schedule_selector(ActionModule::doAction),this,0.01,false);
	return ret;
}

void ActionModule::doAction(float delta)
{	
	if((m_WidgetActionQueue.empty()||m_WidgetActionQueue.size()<1)&&Director::getInstance()->getScheduler()->isScheduled(schedule_selector(ActionModule::doAction),this))
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(ActionModule::doAction),this);
	if(m_WidgetActionQueue.size()<1)
		return;
	WidgetTuple& wt=m_WidgetActionQueue.front();	
	for(int i = 0;i<get<0>(wt).size();i++){		
		Widget* child = get<0>(wt).at(i);
		Vec2 p = child->getPosition();
		child->setVisible(true);
		if(get<2>(wt).equals(Size::ZERO))
			child->setPositionX(child->getPositionX()+child->getSize().width+50);
		else
			child->setPositionX(child->getPositionX()+get<2>(wt).width+50);
		Sequence* seq;
		//EaseBounceIn* bouncein=EaseBounceIn::create(CCMoveTo::create(0.2,p));
		MoveTo* bouncein=MoveTo::create(0.2,p);
		if(get<1>(wt)!=nullptr)
			seq = Sequence::create(DelayTime::create(0.01*(i+1)),bouncein,get<1>(wt)->clone(),NULL);		
		else
			seq = Sequence::create(DelayTime::create(0.01*(i+1)),bouncein,NULL);
		child->runAction(seq);				
	}
	if(get<1>(wt)!=nullptr)
		get<1>(wt)->release();
	m_queue_mtx.lock();
	m_WidgetActionQueue.pop();
	m_queue_mtx.unlock();
}

Sequence* ActionModule::NewAroundAction(Rect rect,Vec2 startPos,CallFuncN* onedone,float livets,AroundType rt)
{
    Vec2 p1=Vec2(rect.getMinX(),rect.getMinY());
    Vec2 p2=Vec2(rect.getMaxX(),rect.getMinY());
    Vec2 p3=Vec2(rect.getMinX(),rect.getMaxY());
    Vec2 p4=Vec2(rect.getMaxX(),rect.getMaxY());
    PointList pl,ppl;
    switch(rt)
    {
        default:
        case rtClockWise:
        {
            pl.push_back(p1);
            pl.push_back(p3);
            pl.push_back(p4);
            pl.push_back(p2);
        }
            break;
        case rtAntiClockWise:
        {
            pl.push_back(p1);
            pl.push_back(p2);
            pl.push_back(p4);
            pl.push_back(p3);
        }
            break;
    }
    return this->NewAroundActionTime(pl,startPos,onedone,livets);
}

Sequence* ActionModule::NewAroundAction(Node* node,Vec2 startPos,CallFuncN* onedone,float livets,AroundType rt)
{
    Size size = node->getContentSize();
    Vec2 p1=Vec2(0,0);
    Vec2 p2=Vec2(size.width,0);
    Vec2 p3=Vec2(0,size.height);
    Vec2 p4=Vec2(size.width,size.height);
    PointList pl,ppl;
    switch(rt)
    {
        default:
        case rtClockWise:
        {
            pl.push_back(p1);
            pl.push_back(p3);
            pl.push_back(p4);
            pl.push_back(p2);
        }
            break;
        case rtAntiClockWise:
        {
            pl.push_back(p1);
            pl.push_back(p2);
            pl.push_back(p4);
            pl.push_back(p3);
        }
            break;
    }
    return this->NewAroundActionTime(pl,startPos,onedone,livets);
}

Sequence* ActionModule::NewAroundActionTime(PointList pl,Vec2 startPos,CallFuncN* onedone,float livets)
{
    if (pl.size()<1) {
        return nullptr;
    }
    
    PointList ppl;
    ppl.clear();
    if (!startPos.equals(Vec2::ZERO)) {
        int start=0;
        for (int i=0; i<pl.size(); i++) {
            if (pl[i].equals(startPos)) {
                start=i;
            }
        }
        for (int i=start; i<pl.size(); i++) {
            ppl.push_back(pl[i]);
        }
        for (int i=0; i<start; i++) {
            ppl.push_back(pl[i]);
        }
    }else
    {
        for (int i=0; i<pl.size(); i++) {
            ppl.push_back(pl[i]);
        }
    }
    
    
    float timelive=livets;
    if (timelive==0) {
        timelive=1.0f;
    }
    LengthList ll;
    ll.clear();
    float alllenth=0;
    for (int i=0; i<ppl.size()-1; i++) {
        ll.push_back(ppl[i].distance(ppl[i+1]));
        alllenth+=ll[i];
    }
    float dist=ppl[ppl.size()-1].distance(ppl[0]);
    ll.push_back(dist);
    alllenth+=dist;
    Vector<FiniteTimeAction*> arrayOfActions;
    for (int i=1; i<ppl.size(); i++) {
        MoveTo* move=MoveTo::create(ll[i]/alllenth*timelive,ppl[i]);
        arrayOfActions.pushBack(move);
    }
    MoveTo* move=MoveTo::create(ll[ll.size()-1]/alllenth*timelive,ppl[0]);
    arrayOfActions.pushBack(move);
    if(onedone!=nullptr)
        arrayOfActions.pushBack(onedone);
    Sequence* Seq = Sequence::create(arrayOfActions);
    return Seq;
}

Sequence* ActionModule::NewAroundActionSpeed(PointList pl,Vec2 startPos,CallFuncN* onedone,float speed)
{
    return nullptr;
}

Sequence* ActionModule::NewHintAnim(float livets,float scale,CallFuncN* onedone,float scaleobj)
{
    ScaleTo* scaleto=ScaleTo::create(0.1,scale);
    ScaleTo* scalereverse=ScaleTo::create(livets,scaleobj);
    
    RotateTo* rotateto=RotateTo::create(0.1, 0);
    RotateTo* rotatereverse=RotateTo::create(livets,0);
    
    Spawn* spawn=Spawn::create(scaleto,rotateto,nullptr);
    Spawn* spawnreverse=Spawn::create(scalereverse, rotatereverse,nullptr);
    Sequence* seq=Sequence::create(DelayTime::create(0.1),spawn,spawnreverse,NULL);
    if (onedone!=nullptr) {
        seq=Sequence::create(seq,onedone,NULL);
    }
    return seq;
}

Sequence* ActionModule::NewBornAnim(float livets,CallFuncN* onedone,float scaleobj)
{
    float time=livets;
    if (time<=0) {
        livets=0.5;
    }
    Sequence* seq=Sequence::create(ScaleTo::create(0.0, 0.1*scaleobj),ScaleTo::create(livets, 1.0*scaleobj),onedone, NULL);
    return seq;
}


