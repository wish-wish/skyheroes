/******************************************************************************** 
 * create：cclin
 * date：2014-11-5
 * ver：
 * latest update：
 * function：
 * update log：
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "OvalActionInterval.h"
 
MoveOvalBy::MoveOvalBy(){
 
}
 
//
//MoveOvalBy
//
MoveOvalBy* MoveOvalBy::create(float t, const OvalConfig& c,float byAngle){//利用之前定义的椭圆的参数初始化椭圆
	MoveOvalBy *action = new MoveOvalBy();
	action->initWithDuration(t, c);
	action->autorelease();  
	return action;
}
 
bool MoveOvalBy::initWithDuration(float t, const OvalConfig& c,float byAngle){
	if (ActionInterval::initWithDuration(t)){
		_config = c;
		_angle = byAngle;
		return true;
	} 
	return false;
}

void MoveOvalBy::update(float t){
	//t [0, 1]
	//log("t:%f", t);
	if (_target){
		float x = getPositionXAtOval(t);//调用之前的坐标计算函数来计算出坐标值
		float y = getPositionYAtOval(t);
		_target->setPosition(_config.centerPosition + Vec2(x, y));//由于我们画计算出的椭圆你做值是以原点为中心的，所以需要加上我们设定的中心点坐标
 
		if(t <= 0.5){
			_target->setLocalZOrder(_config.zOrder.first);
		}else{
			_target->setLocalZOrder(_config.zOrder.second);
		}
	}
}
 
MoveOvalBy* MoveOvalBy::clone() const{
	auto action = new MoveOvalBy();
	action->initWithDuration(_duration, _config);
	action->autorelease();
	return action;
}
 
MoveOvalBy* MoveOvalBy::reverse() const{
	OvalConfig newConfig;
	newConfig.centerPosition = _config.centerPosition;
	newConfig.a = _config.a;
	newConfig.b = _config.b;
	newConfig.moveInAnticlockwise = !_config.moveInAnticlockwise;
	newConfig.zOrder = _config.zOrder;
	return MoveOvalBy::create(_duration, newConfig);
}
 
void MoveOvalBy::startWithTarget(Node *target){
	ActionInterval::startWithTarget(target);
}

float MoveOvalBy::getPositionXAtOval(float t)
{
	//参数方程
	if(_config.moveInAnticlockwise == false){
		return _config.a * cos(_angle * (1 - t));
	}else{
		return _config.a * cos(_angle * t);
	}  
}

float MoveOvalBy::getPositionYAtOval(float t)
{
	//参数方程
	if(_config.moveInAnticlockwise == false){
		return _config.b * sin(_angle * (1 - t));
	}else{
		return _config.b * sin(_angle * t);
	}
}

float MoveOvalBy::getAngleAtPos(Vec2 vec)
{
	float dist=vec.distance(_config.centerPosition);
	Vec2 offset=vec-_config.centerPosition;
	return offset.getAngle();
}

MoveOvalTo* MoveOvalTo::create(float t, const OvalConfig& c,float ToAngle)
{
	MoveOvalTo* ovalto=new MoveOvalTo();
	ovalto->initWithDuration(t,c,ToAngle);
	ovalto->autorelease();
	return ovalto;
}

void MoveOvalTo::startWithTarget(Node *target)
{
	MoveOvalBy::startWithTarget(target);	
	Vec2 vec=target->getPosition()-_config.centerPosition;	
	_angle = _ToAngle-vec.getAngle();
}

MoveOvalTo* MoveOvalTo::clone() const 
{
	auto a= new MoveOvalTo();
	a->initWithDuration(_duration,_config,_ToAngle);
	a->autorelease();
	return a;
}

MoveOvalTo* MoveOvalTo::reverse(void) const 
{
	CCASSERT(false, "MoveOvalTo doesn't support the 'reverse' method");
	return nullptr;
}

bool MoveOvalTo::initWithDuration(float t, const OvalConfig &c,float ToAngle)
{
	if(MoveOvalBy::initWithDuration(t,c,ToAngle))
	{
		_ToAngle = ToAngle;
		return true;
	}
	return false;
}

LRTuoyuanBy* LRTuoyuanBy::create(float t, const lrTuoyuanConfig& c)//利用之前定义的椭圆的三个参数初始化椭圆  
{  
    LRTuoyuanBy *pTuoyuanBy = new LRTuoyuanBy();  
    pTuoyuanBy->initWithDuration(t, c);  
    pTuoyuanBy->autorelease();  
  
    return pTuoyuanBy;  
}  
  
bool LRTuoyuanBy::initWithDuration(float t, const lrTuoyuanConfig& c)  
{  
    if (ActionInterval::initWithDuration(t))  
    {  
        m_sConfig = c;  
        return true;  
    }  
  
    return false;  
}  
void LRTuoyuanBy::update(float time)  
{  
    if (_target)  
    {  
        Vec2 s_startPosition = m_sConfig.centerPosition;//中心点坐标  
        float a = m_sConfig.aLength;  
        float bx = m_sConfig.centerPosition.x;  
        float by = m_sConfig.centerPosition.y;  
        float c = m_sConfig.cLength;  
        float x = tuoyuanXat(a, bx, c, time);//调用之前的坐标计算函数来计算出坐标值  
        float y = tuoyuanYat(a, by, c, time);  
        _target->setPosition(s_startPosition+Vec2(x-a, y));//由于我们画计算出的椭圆你做值是以原点为中心的，所以需要加上我们设定的中心点坐标  
    }  
} 

float LRTuoyuanBy::tuoyuanXat(float a, float bx, float c, float t)
{
	//参数方程  
	return -a*cos(2*OvalPI*t)+a;
}

float LRTuoyuanBy::tuoyuanYat(float a, float by, float c, float t)
{
	float b = sqrt(powf(a, 2) - powf(c, 2));//因为之前定义的参数是焦距c而不是短半轴b，所以需要计算出b  
	//参数方程  
	return b*sin(2*OvalPI*t);
}

LRTuoyuanBy* LRTuoyuanBy::reverse() const 
{
	lrTuoyuanConfig newConfig;
	newConfig.centerPosition = m_sConfig.centerPosition;
	newConfig.aLength = m_sConfig.aLength;
	newConfig.cLength = m_sConfig.cLength;
	//newConfig.moveInAnticlockwise = !_config.moveInAnticlockwise;
	//newConfig.zOrder = _config.zOrder;
	return LRTuoyuanBy::create(_duration, m_sConfig);
}

LRTuoyuanBy* LRTuoyuanBy::clone() const 
{
	auto action = new LRTuoyuanBy();
	action->initWithDuration(_duration, m_sConfig);
	action->autorelease();
	return action;
}


