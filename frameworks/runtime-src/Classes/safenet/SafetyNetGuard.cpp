/******************************************************************************** 
 * create:cclin
 * date:2014-12-03
 * ver:
 * latest update:
 * function:Õ¯¬Á ÿª§
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetyNetGuard.h"

SafetyNetGuard::SafetyNetGuard(SafetyLongSocket* longsocket)
{	
	m_LongSocket=longsocket;
    m_IsReconnect=true;
    m_Interval=1.0;
    m_ReconnTimes=6;
    m_FailTimes = 18;
    m_CountCycles = 0;
    m_OldStatus = csConnUnkown;
    m_OldControl = ccUnknown;
    if(!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(SafetyNetGuard::RenderFunc),this))
        Director::getInstance()->getScheduler()->schedule(schedule_selector(SafetyNetGuard::RenderFunc),this,m_Interval,false);
}

SafetyNetGuard::~SafetyNetGuard()
{
    if(Director::getInstance()->getScheduler()->isScheduled(schedule_selector(SafetyNetGuard::RenderFunc),this))
        Director::getInstance()->getScheduler()->unschedule(schedule_selector(SafetyNetGuard::RenderFunc),this);
}

void SafetyNetGuard::setReconnect(bool is)
{
    m_IsReconnect = is;
    
    SocketAddress * addr=m_LongSocket->getAddress();
    m_OldStatus = addr->m_ConnStatus;
    m_OldControl = addr->m_UserControl;
}

void SafetyNetGuard::setDetectInterval(float interval)
{
    m_Interval = interval;
    Director::getInstance()->getScheduler()->unschedule(schedule_selector(SafetyNetGuard::RenderFunc),this);
    Director::getInstance()->getScheduler()->schedule(schedule_selector(SafetyNetGuard::RenderFunc),this,m_Interval,false);
    
    SocketAddress * addr=m_LongSocket->getAddress();
    m_OldStatus = addr->m_ConnStatus;
    m_OldControl = addr->m_UserControl;
}

void SafetyNetGuard::setReconnectTimes(int Times)
{
    m_ReconnTimes=Times;
}

void SafetyNetGuard::setFailTimes(int Times)
{
    m_FailTimes = Times;
}

void SafetyNetGuard::RenderFunc(float delta)
{
    //check if socket has error or close
    m_LongSocket->checkSocket();
    if (!m_IsReconnect) return;
    m_CountCycles++;
    
    SocketAddress * addr=m_LongSocket->getAddress();

    if(addr->m_UserControl==ccOpen) {
        switch (addr->m_ConnStatus) {
            case csConnSuccess:
            {
                //nothing to do
                m_CountSuccess++;
            }
                break;
            case csAutoReconnect:
            {
                m_CountReconnect++;
                if (m_CountReconnect % m_ReconnTimes==0) {
                    addr->m_ConnStatus = csReconnDialog;
                }
            }
                break;
            case csConnError:
            {
                m_CountReconnect++;
                addr->m_ConnStatus = csAutoReconnect;
            }
                break;
            case csDisconnect:
            {
                m_CountReconnect++;
                addr->m_ConnStatus = csAutoReconnect;
            }
                break;
            case csConnClose:
            {
                m_CountReconnect++;
                addr->m_ConnStatus = csAutoReconnect;
            }
                break;
            case csConnFail:
            {
                m_CountReconnect++;
                addr->m_ConnStatus = csAutoReconnect;
            }
                break;
            case csReconnDialog:
            {
                m_CountReconnect++;
                if (m_CountReconnect % m_FailTimes==0) {
                    addr->m_ConnStatus = csFailDialog;
                }
            }
                break;
            case csFailDialog://not net
            {
                //
            }
                break;
            case csConnecting://connecting
            {
                //
            }
                break;
            case csConnUnkown:
            {
                
            }
                break;
            default:
            {
                
            }
                break;
        }
        if (m_OldStatus!=addr->m_ConnStatus) {
            if (m_OldStatus==csConnSuccess&&addr->m_ConnStatus!=csConnSuccess) {
                this->LostConnected(m_OldStatus, addr->m_ConnStatus);
            }else if (m_OldStatus!=csConnSuccess&&addr->m_ConnStatus==csConnSuccess) {
                this->ConnectedSuccess(m_OldStatus, addr->m_ConnStatus);
            }else if (m_OldStatus!=csReconnDialog&&addr->m_ConnStatus==csReconnDialog) {
                if(m_IsReconnect)
                    this->ShowReconnectDialog();
                else
                {
                    addr->m_UserControl = ccClose;
                    CCLOG("can't connect,reconn,close it!");
                }
            }else if (m_OldStatus!=csFailDialog&&addr->m_ConnStatus==csFailDialog) {
                if(m_IsReconnect)
                    this->ShowFailDialog();
                else
                {
                    addr->m_UserControl = ccClose;
                    CCLOG("can't connect,fail,close it!");
                }
            }
            CCLOG("status change from %d to %d!",m_OldStatus,addr->m_ConnStatus);
        }
        if (addr->m_ConnStatus==csAutoReconnect) {
            this->Reconnect();
        }
    }//ccOpen
    m_OldStatus = addr->m_ConnStatus;
    m_OldControl = addr->m_UserControl;
}

void SafetyNetGuard::LostConnected(ConnectStatus oldstatus,ConnectStatus newstatus)
{
    m_CountReconnect = 0;
    SocketAddress * addr=m_LongSocket->getAddress();
    if (m_IsReconnect) {
        addr->m_ConnStatus = csAutoReconnect;
    }
    else
    {
        addr->m_ConnStatus = csConnClose;
    }
}


void SafetyNetGuard::ConnectedSuccess(ConnectStatus oldstatus,ConnectStatus newstatus)
{
    m_CountSuccess = 0;
    //hide loading or dialog
}

void SafetyNetGuard::Reconnect()
{
    m_LongSocket->Reconnect();
}
    
void SafetyNetGuard::ShowFailDialog()
{
    CCLOG("SafetyNetGuard::ShowFialDialog");
}

void SafetyNetGuard::ShowReconnectDialog()
{
    CCLOG("SafetyNetGuard::ShowReconnectDialog");
}



