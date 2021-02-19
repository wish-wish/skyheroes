/******************************************************************************** 
 * create:cclin
 * date:2014-12-03
 * ver:
 * latest update:
 * function:Õ¯¬Á ÿª§
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "SafetyLongSocket.h"
#include "cocos2d.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;

class SafetyNetGuard:public Node
{
public:
	explicit SafetyNetGuard(SafetyLongSocket* longsocket);
	~SafetyNetGuard();
    void setReconnect(bool is);
    void setDetectInterval(float interval);
    void setReconnectTimes(int Times);
    void setFailTimes(int Times);
    
    SafetyLongSocket* getLongSocket(){return m_LongSocket;};
private:
    bool m_IsReconnect;
    float  m_Interval;
    int  m_ReconnTimes;
    int  m_FailTimes;
    unsigned int m_CountCycles;
    
    unsigned int m_CountSuccess;
    unsigned int m_CountReconnect;
    
    ConnectStatus m_OldStatus;
    ConnectControl m_OldControl;
    void RenderFunc(float delta);
	SafetyLongSocket* m_LongSocket;
    
    void LostConnected(ConnectStatus oldstatus,ConnectStatus newstatus);
    void ConnectedSuccess(ConnectStatus oldstatus,ConnectStatus newstatus);
    
    void ShowFailDialog();
    void ShowReconnectDialog();
    
    void Reconnect();
};