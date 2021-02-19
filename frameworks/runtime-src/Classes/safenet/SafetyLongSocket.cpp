/******************************************************************************** 
 * create:cclin
 * date:2014-11-26
 * ver:
 * latest update:
 * function:³¤Á¬½Ó
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetyLongSocket.h"

SafetyLongSocket::SafetyLongSocket(string ip,int port,string name,int tag)
{	
	m_Addr.nServerIp = ip;
	m_Addr.nPort = port;	
	m_Addr.nTag = tag;
	m_Addr.nName = name;
	m_Addr.nServerName = "wanghu";
    m_Addr.m_UserControl = ccOpen;
}

SafetyLongSocket::~SafetyLongSocket()
{
	while (!m_RecvQueue.empty())
	{
		m_RecvQueue.pop();
	}	
}

bool SafetyLongSocket::checkSocket()
{
    return m_Socket->Check();
}

bool SafetyLongSocket::SendDataInstant(void* data,int size)
{
	if(m_Addr.m_ConnStatus!=csConnSuccess)
	{
        m_send_queue_mtx.lock();
        m_SendQueue.push(new WorldPacket(data,size));
        m_send_queue_mtx.unlock();
		this->connect(&m_Addr);
	}
	if(m_Addr.m_ConnStatus==csConnSuccess)
	{
		if(m_Socket->SendMsg(data,size)&&m_Socket->Flush())
		{
			CCLOG("SafetyLongSocket::SendData Success");
			return true;			
		}
		else
		{
			CCLOG("SafetyLongSocket::SendData Fail");
		}		
	}else
	{
		CCLOG("SafetyLongSocket::connect Fail");
	}
	return false;
}

bool SafetyLongSocket::SendData(void* data,int size)
{	
	WorldPacket* wp=new WorldPacket(data,size);
	this->SendData(wp);
	delete wp;
	return true;
}

bool SafetyLongSocket::SendData(WorldPacket* wp)
{
	m_send_queue_mtx.lock();
	m_SendQueue.push(wp->clone());
	m_send_queue_mtx.unlock();
	if(m_Addr.m_ConnStatus!=csConnSuccess)
	{
		this->connect(&m_Addr);
	}
	return true;
}

void SafetyLongSocket::EnterDataFunc(sckDataFunc func)
{
	m_Addr.m_DataFunc = func;
}

void SafetyLongSocket::ExitDataFunc()
{
	m_Addr.m_DataFunc = nullptr;
}

bool SafetyLongSocket::connect(SocketAddress *pAddress)
{
	SafetySocket *pSocket;
	if (pAddress->nServerName.compare("firefly") == 0)
	{
		pSocket = new SafetySocket_firefly();
	}
	else if (pAddress->nServerName.compare("wanghu") == 0)
	{
		pSocket = new SafetySocket_wanghu();
	}
	else
	{
		pSocket = new SafetySocket();
	}
	if (pSocket->Create(pAddress))
	{		
		pAddress->nState = 1;
		pAddress->nReConnectCount = 0;		
		m_Socket = pSocket;
		return true;
	}
	else
	{	
		pAddress->nReConnectCount += 1;
		pAddress->nState = 0;		
	}
	delete pSocket;
	return false;
}

bool SafetyLongSocket::ProcessRecv()
{
	if(m_Addr.m_ConnStatus!=csConnSuccess)
		return false;
	char buffer[_MAX_MSGSIZE] = {0};
	memset(buffer, 0, _MAX_MSGSIZE);
	int nSize = sizeof(buffer);
	char *pbufMsg = buffer;
	if (!m_Socket->ReceiveMsg(pbufMsg, nSize))
		return false;	
	WorldPacket* packet=new WorldPacket(pbufMsg,nSize);
	m_recv_queue_mtx.lock();
	m_RecvQueue.push(packet);
	m_recv_queue_mtx.unlock();
	return true;
}

bool SafetyLongSocket::ProcessSend()
{
	if(m_Addr.m_ConnStatus!=csConnSuccess)
		return false;
	unsigned int count=min(ProcessSendCount,m_SendQueue.size());
	for(int i=0;i<count;i++)
	{
		WorldPacket* wp=m_SendQueue.top();
		if(m_Socket->SendMsg(wp->contents(),wp->size())&&m_Socket->Flush())
		{
			m_send_queue_mtx.lock();
			m_SendQueue.pop();
			m_send_queue_mtx.unlock();
		}else
		{
			wp->m_failcount+=1;
			if(wp->m_failcount>RearCountTimes)
			{
				wp->m_priority = 0;
			}							
			if(wp->m_failcount>BadCountTimes)
			{
				WorldPacket* np=wp->clone();
				m_SendQueue.pop();
				m_BadQueue.push(np);
				CCLOG("SafetyLongSocket::send Move to Bad Queue");
			}else
			{
				m_send_queue_mtx.lock();
				m_SendQueue.pop_push();				
				m_send_queue_mtx.unlock();
			}
		}
	}
	return true;
}

void SafetyLongSocket::Reconnect()
{
    m_Socket->Destroy();
    this->connect(&m_Addr);
}

void SafetyLongSocket::CloseSocket()
{
    m_Addr.m_UserControl = ccClose;
    this->destory();
}

void SafetyLongSocket::destory()
{
    m_Socket->Destroy();
    WorldPacket* wp=new WorldPacket();
    wp->m_command = 999;
    wp->m_tag     = m_Socket->GetSocket();
    m_recv_queue_mtx.lock();
    m_RecvQueue.push(wp);
    m_recv_queue_mtx.unlock();
}

void SafetyLongSocket::DispatchData()
{
	int count=min(ProcessRecvCount,m_RecvQueue.size());
	for(int i=0;i<count;i++)
	{
		if(m_Addr.m_DataFunc!=nullptr)
		{
			WorldPacket* wp=m_RecvQueue.top();
			if(m_Addr.m_DataFunc(m_Socket->getAddress(),wp))
			{				
				m_recv_queue_mtx.lock();
				m_RecvQueue.pop();
				m_recv_queue_mtx.unlock();				
			}
			else
			{				
				m_recv_queue_mtx.lock();
				m_RecvQueue.pop_push();
				m_recv_queue_mtx.unlock();				
			}
		}
	}	
}

void SafetyLongSocket::RenderUpdate(float delta)
{
	this->DispatchData();
}

void SafetyLongSocket::EnterConnFunc(sckConnectFunc func)
{
	m_Addr.m_ConnectFunc = func;
}

void SafetyLongSocket::ExitConnFunc()
{
	m_Addr.m_ConnectFunc = nullptr;
}

unsigned int SafetyLongSocket::GetHandle()
{
	return m_Addr.nHandle;
}




