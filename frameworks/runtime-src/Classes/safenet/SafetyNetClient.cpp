/******************************************************************************** 
 * create:cclin
 * date:2014-11-24
 * ver:
 * latest update:
 * function:°²È«ÍøÂç
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetyNetClient.h"
#include "cocostudio/CocoStudio.h"
#include <chrono>    // std::chrono::seconds
#include <thread>    // std::thread, std::this_thread::sleep_for
#include "SystemConfig.h"

using namespace cocostudio;

const unsigned int BadCountTimes=1000;
const unsigned int RearCountTimes=6;
const unsigned int ProcessSendCount=5;
const unsigned int ProcessRecvCount=3;

SafetyNetClient::SafetyNetClient()
{
	m_isClose = false;
	m_isStarted = false;	
	if(!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(SafetyNetClient::RenderFunc),this))
		Director::getInstance()->getScheduler()->schedule(schedule_selector(SafetyNetClient::RenderFunc),this,0.0f,false);
	this->retain();
	m_DataFunc = nullptr;
#ifdef WIN32
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	int ret = WSAStartup(version, &wsaData);
	CCLOG("SafetyNetClient Clean wsa ret=%d",ret);
#endif

}

SafetyNetClient::~SafetyNetClient()
{
#ifdef WIN32	
	int ret=WSACleanup();
	CCLOG("SafetyNetClient Clean wsa ret=%d",ret);
#endif
	m_isClose = true;	
	m_DataFunc = nullptr;
	if(Director::getInstance()->getScheduler()->isScheduled(schedule_selector(SafetyNetClient::RenderFunc),this))
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(SafetyNetClient::RenderFunc),this);	
	this->release();
}

bool SafetyNetClient::SendPacket(WorldPacket* wp,string name)
{
    SafetyLongSocket* socket=this->getSocket(name);
    if(socket==nullptr)
        return false;
    socket->SendData(wp);
    return true;
}

bool SafetyNetClient::SendData(void* data,unsigned int s,string name)
{
    SafetyLongSocket* socket=this->getSocket(name);
    if(socket==nullptr)
        return false;
    WorldPacket* wp=new WorldPacket(data,s);
    socket->SendData(wp);
    return true;
}

void SafetyNetClient::ThreadFunc(void* param)
{
	CCLOG("SafetyNetClient::ThreadFunc Start %ld",(long int)param);
	while(!m_isClose)
	{
#ifdef WIN32
		Sleep(20);
#else
		usleep(20*100);
#endif
		this->ProcessSend();
		this->ProcessRecv();
	}
	CCLOG("SafetyNetClient::ThreadFunc Exit %ld",(long int)param);
	m_isStarted = false;
}

void SafetyNetClient::StartThread(int param)
{	
	if(!m_isStarted)
	{
		m_isStarted = true;
		thread SafetyNetThread(&SafetyNetClient::ThreadFunc,this,(void*)param);
		SafetyNetThread.detach();
		CCLOG("SafetyNetClient::StartThread Created & Start");
	}
	else
	{
		CCLOG("SafetyNetClient::StartThread Already Running");
	}
}


void SafetyNetClient::ProcessSend()
{
    TLongSocketList::iterator iter=m_LongSocketList.begin();
    while (iter!=m_LongSocketList.end())
    {
        (*iter)->ProcessSend();
        iter++;
    }
}

void SafetyNetClient::RenderFunc(float delta)
{
	for(int i=0;i<m_ShortSocketList.size();i++)
		m_ShortSocketList[i]->RenderUpdate(delta);
    for(int i=0;i<m_LongSocketList.size();i++)
        m_LongSocketList[i]->RenderUpdate(delta);
    
	if(m_NetClientType==nstThread||m_isClose)
		return;
	for(unsigned int i=0;i<ProcessSendCount;i++)
	{
		this->ProcessSend();
	}
	for(unsigned int i=0;i<ProcessRecvCount;i++)
	{
		this->ProcessRecv();
	}
}

void SafetyNetClient::Start(NetClientType nct,int param)
{
    CMessageQueue::getInstance()->subcribeQueue(this, qn_net_send);
	m_NetClientType=nct;
	m_isClose = false;
	if(m_NetClientType==nstThread)
		this->StartThread(param);
	else if(m_NetClientType==nstRender)
	{
        m_isClose = false;
		CCLOG("SafetyNetClient::Start RenderNet");
	}
}

void SafetyNetClient::Stop()
{
	m_isClose = true;
}

void SafetyNetClient::ProcessRecv()
{
    TLongSocketList::iterator iter=m_LongSocketList.begin();
    while (iter!=m_LongSocketList.end())
    {
        (*iter)->ProcessRecv();
        iter++;
    }
}

void SafetyNetClient::EnterDataFunc(sckDataFunc func)
{
	m_DataFunc = func;
}

void SafetyNetClient::ExitDataFunc()
{
	m_DataFunc = nullptr;
}

bool SafetyNetClient::DataCall(SocketAddress* addr,WorldPacket* wp)
{
    if(wp->m_command==999)//close the socket
    {
        this->DelSocket(wp->m_tag);//close the socket
        return true;
    }
    CMessageQueue::getInstance()->sendMessage(this, qn_net_recv, wp);
	if(m_DataFunc!=nullptr)
		return m_DataFunc(addr,wp);
	else
		return false;
}


SafetyShortSocket* SafetyNetClient::NewShortSocket(string ip,int port,string name)
{
    int tag=SocketAddress::getMaxTag();
    CCLOG("NewShortSocket %s %d %s tag=%d",ip.c_str(),port,name.c_str(),tag);
    SafetyShortSocket* shortscoket=new SafetyShortSocket(ip,port,name,tag);
    shortscoket->EnterDataFunc(SCT_CALLBACK_2(SafetyNetClient::DataCall,this));
    m_ShortSocketList.push_back(shortscoket);
    return shortscoket;
}

void SafetyNetClient::DelShortSocket(string ip,int port)
{
    CCLOG("DelShortSocket %s %d",ip.c_str(),port);
    TShortSocketList::iterator iter=m_ShortSocketList.begin();
    while (iter!=m_ShortSocketList.end())
    {
        if((*iter)->GetIP().compare(ip)==0&&(*iter)->GetPort()==port)
        {
            (*iter)->CloseSocket();
            (*iter)->ExitDataFunc();
            this->DelHttpSocket(*iter);
            delete (*iter);
            m_ShortSocketList.erase(iter);
            break;
        }
        iter++;
    }
}

SafetyHttpSocket* SafetyNetClient::NewHttpSocket(string ip,int port,string name)
{
    CCLOG("NewHttpSocket %s %d %s",ip.c_str(),port,name.c_str());
    SafetyHttpSocket* httpscoket=new SafetyHttpSocket(ip,port,name);
    httpscoket->EnterDataFunc(SCT_CALLBACK_2(SafetyNetClient::DataCall,this));
    m_HttpSocketList.push_back(httpscoket);
    return httpscoket;
}


SafetyLongSocket* SafetyNetClient::NewLongSocket(string ip,int port,string name)
{
    int tag=this->getSocketTag(ip,port);
	int stag = this->getMaxTag();
	CCLOG("NewLongSocket %s %d %s tag=%d", ip.c_str(), port, name.c_str(), stag);
    SafetyLongSocket* longsocket;
    if(tag==-1)
    {
		longsocket = new SafetyLongSocket(ip, port, name, stag);
        longsocket->EnterDataFunc(SCT_CALLBACK_2(SafetyNetClient::DataCall,this));
        m_LongSocketList.push_back(longsocket);
        
        //SafetyNetGuard* netguard=new SafetyNetGuard(longsocket);
        //m_NetGuardList.push_back(netguard);
    }else
    {
        longsocket=this->getSocket(tag);
    }
    return longsocket;
}

void SafetyNetClient::DelLongSocket(string ip,int port)
{
    CCLOG("DelLongSocket %s %d",ip.c_str(),port);
    TLongSocketList::iterator iter=m_LongSocketList.begin();
    while (iter!=m_LongSocketList.end())
    {
        if((*iter)->GetIP().compare(ip)==0&&(*iter)->GetPort()==port)
        {
            (*iter)->CloseSocket();
            (*iter)->ExitDataFunc();
            this->DelNetGuard(*iter);
            delete (*iter);
            m_LongSocketList.erase(iter);
            break;
        }
        iter++;
    }
}

void SafetyNetClient::DelNetGuard(SafetyLongSocket *longsocket)
{
    TNetGuardList::iterator iter=m_NetGuardList.begin();
    while (iter!=m_NetGuardList.end())
    {
        if((*iter)->getLongSocket()==longsocket)
        {
            delete (*iter);
            m_NetGuardList.erase(iter);
            break;
        }
        iter++;
    }
}

void SafetyNetClient::DelHttpSocket(SafetyShortSocket *shortsocket)
{
    THttpSocketList::iterator hiter=m_HttpSocketList.begin();
    while(hiter!=m_HttpSocketList.end())
    {
        if((*hiter)->getShortSocket()==shortsocket)
        {
            (*hiter)->ExitDataFunc();
            delete(*hiter);
            m_HttpSocketList.erase(hiter);
            break;
        }
        hiter++;
    }
}

void SafetyNetClient::DelSocket(int socket)
{
    TShortSocketList::iterator iter=m_ShortSocketList.begin();
    while (iter!=m_ShortSocketList.end())
    {
        if((*iter)->getSocket()->GetSocket()==socket)
        {
            (*iter)->CloseSocket();
            (*iter)->ExitDataFunc();
            delete (*iter);
            m_ShortSocketList.erase(iter);
            break;
        }
        iter++;
    }
    TLongSocketList::iterator liter=m_LongSocketList.begin();
    while (liter!=m_LongSocketList.end())
    {
        if((*liter)->getSocket()->GetSocket()==socket)
        {
            (*liter)->CloseSocket();
            (*liter)->ExitDataFunc();
            delete (*liter);
            m_LongSocketList.erase(liter);
            break;
        }
        liter++;
    }
    THttpSocketList::iterator hiter=m_HttpSocketList.begin();
    while(hiter!=m_HttpSocketList.end())
    {
        if((*hiter)->getShortSocket()->getSocket()->GetSocket()==socket)
        {
            (*hiter)->ExitDataFunc();
            delete(*hiter);
            m_HttpSocketList.erase(hiter);
            break;
        }
        hiter++;
    }
    TNetGuardList::iterator giter=m_NetGuardList.begin();
    while(giter!=m_NetGuardList.end())
    {
        if((*giter)->getLongSocket()->getSocket()->GetSocket()==socket)
        {
            delete(*giter);
            m_NetGuardList.erase(giter);
            break;
        }
        giter++;
    }
}

SafetyLongSocket* SafetyNetClient::getSocket(string name)
{
	TLongSocketList::iterator iter=m_LongSocketList.begin();
	while (iter!=m_LongSocketList.end())
	{
		if((*iter)->getName().compare(name)==0)
		{			
			return (*iter);						
		}
		iter++;
	}
	return nullptr;
}

SafetyLongSocket* SafetyNetClient::getSocket(int tag)
{
	TLongSocketList::iterator iter=m_LongSocketList.begin();
	while (iter!=m_LongSocketList.end())
	{
		if((*iter)->getAddress()->nTag==tag)
		{			
			return (*iter);						
		}
		iter++;
	}
	return nullptr;
}

SafetyLongSocket* SafetyNetClient::getSocket(string ip,int port)
{
    TLongSocketList::iterator iter=m_LongSocketList.begin();
    while (iter!=m_LongSocketList.end())
    {
        if((*iter)->GetIP().compare(ip)==0&&(*iter)->GetPort()==port)
        {
            return (*iter);
        }
        iter++;
    }
    return nullptr;
}

int SafetyNetClient::getSocketTag(string ip,int port)
{
	TLongSocketList::iterator iter=m_LongSocketList.begin();
	while (iter!=m_LongSocketList.end())
	{
		if((*iter)->GetIP().compare(ip)==0&&(*iter)->GetPort()==port)
		{
			return (*iter)->getAddress()->nTag;			
		}
		iter++;
	}
	return -1;
}

int SafetyNetClient::getMaxTag()
{
	TLongSocketList::iterator iter = m_LongSocketList.begin();
	int tag = -1;
	while (iter != m_LongSocketList.end())
	{
		if (tag<(*iter)->getAddress()->nTag)
		{
			tag = (*iter)->getAddress()->nTag;
		}
		iter++;
	}
	return tag + 1;
}

string SafetyNetClient::getSocketName(string ip, int port)
{
    TLongSocketList::iterator iter=m_LongSocketList.begin();
    while (iter!=m_LongSocketList.end())
    {
        if((*iter)->GetIP().compare(ip)==0&&(*iter)->GetPort()==port)
        {
            return (*iter)->getAddress()->nName;
        }
        iter++;
    }
    return "";
}


bool SafetyNetClient::onMessage(CMessageSender* sender, WorldPacket* packet, int msg_id)
{
    //section by msg_id to difference server
    return this->SendPacket(packet,"Gateway");
}





