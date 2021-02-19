/******************************************************************************** 
 * create:cclin
 * date:2014-12-03
 * ver:
 * latest update:
 * function:ÍøÂçÊØ»¤
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetyHttpSocket.h"
#include "SafetyNetClient.h"


SafetyHttpSocket::SafetyHttpSocket(string ip,int port,string name)
{
    m_ShortSocket = SafetyNetClient::getInstance()->NewShortSocket(ip,port,name);
    m_ShortSocket->EnterDataFunc(SCT_CALLBACK_2(SafetyHttpSocket::DataCall,this));
    m_ShortSocket->EnterConnFunc(SCT_CALLBACK_2(SafetyHttpSocket::ConnectCall,this));
    m_HttpDataFunc = nullptr;
}

SafetyHttpSocket::~SafetyHttpSocket()
{
    
}

void SafetyHttpSocket::EnterDataFunc(sckDataFunc func)
{
    m_HttpDataFunc = func;
}
void SafetyHttpSocket::ExitDataFunc()
{
    m_HttpDataFunc = nullptr;
}

void SafetyHttpSocket::RenderFunc(float delta)
{
    
}

bool SafetyHttpSocket::SendHeader(string header)
{
    //todo:
    return false;
}
bool SafetyHttpSocket::SendBody(string body)
{
    //todo:
    return false;
}

WorldPacket* SafetyHttpSocket::HttpParseContent(WorldPacket* wp)
{
	//parse
    return wp;
}

void SafetyHttpSocket::ConnectCall(SocketAddress* addr,ConnectStatus cs)
{
    //
}

bool SafetyHttpSocket::DataCall(SocketAddress* addr,WorldPacket* wp)
{
    if(wp->m_command==999)//command,direct connect
    {
        m_HttpDataFunc(addr,wp);
        return true;
    }
    WorldPacket* pwp=this->HttpParseContent(wp);
    if(m_HttpDataFunc)
    {
        return m_HttpDataFunc(addr,pwp);
    }
    return false;
}




