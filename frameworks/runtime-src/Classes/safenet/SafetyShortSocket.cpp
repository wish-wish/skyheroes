/******************************************************************************** 
 * create:cclin
 * date:2014-11-26
 * ver:
 * latest update:
 * function:∂Ã¡¨Ω”
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetyShortSocket.h"

SafetyShortSocket::SafetyShortSocket(string ip,int port,string name,int tag)
{	
	m_Addr.nServerIp = ip;
	m_Addr.nPort = port;
	m_Addr.nTag = tag;
	m_Addr.nName = name;
	m_Close = false;
    m_isThreadStart = false;
}

SafetyShortSocket::~SafetyShortSocket()
{
	m_Close = true;
    m_isThreadStart = false;
	while (!m_RecvQueue.empty())
	{
		m_RecvQueue.pop();
	}	
}

bool SafetyShortSocket::SendData(void* data,int size)
{
	if(this->connect(&m_Addr))
	{
		if(m_Socket->SendMsg(data,size)&&m_Socket->Flush())
		{
			//start recv thread
            if(!m_isThreadStart)
            {
                thread SafetyNetThread(&SafetyShortSocket::recvthread,this,(void*)this);
                SafetyNetThread.detach();
            }
			CCLOG("SafetyShortSocket::SendData Success");
			return true;			
		}else
		{
			CCLOG("SafetyShortSocket::SendData Fail");
		}		
	}else
	{
		CCLOG("SafetyShortSocket::connect Fail");
	}
	return false;
}

void SafetyShortSocket::EnterDataFunc(sckDataFunc func)
{
	m_Addr.m_DataFunc = func;
}

void SafetyShortSocket::ExitDataFunc()
{
	m_Addr.m_DataFunc = nullptr;
}

bool SafetyShortSocket::connect(SocketAddress *pAddress)
{
	SafetySocket_firefly *pSocket = new SafetySocket_firefly();
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

bool SafetyShortSocket::ProcessRecv()
{
	char buffer[_MAX_MSGSIZE] = {0};
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

void SafetyShortSocket::CloseSocket()
{
	m_Close = true;
}

void SafetyShortSocket::destory()
{
    m_Socket->Destroy();
    WorldPacket* wp=new WorldPacket();
    wp->m_command = 999;
    wp->m_tag     = m_Socket->GetSocket();
    m_recv_queue_mtx.lock();
    m_RecvQueue.push(wp);
    m_recv_queue_mtx.unlock();
}

void SafetyShortSocket::DispatchData()
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

void SafetyShortSocket::RenderUpdate(float delta)
{
	this->DispatchData();
}

void SafetyShortSocket::recvthread(void* sss)
{
    m_isThreadStart = true;
	CCLOG("SafetyShortSocket recv Start");
	while(m_Addr.m_ConnStatus==csConnSuccess){
#ifdef WIN32
		Sleep(20);
#else
		usleep(20*1000);
#endif
		this->ProcessRecv();
	}
	CCLOG("SafetyShortSocket recv Exit");
	this->destory();
    m_isThreadStart = false;
}

unsigned int SafetyShortSocket::GetHandle()
{
	return m_Addr.nHandle;
}

void SafetyShortSocket::EnterConnFunc(sckConnectFunc func)
{
	m_Addr.m_ConnectFunc = func;
}

void SafetyShortSocket::ExitConnFunc()
{
	m_Addr.m_ConnectFunc = nullptr;
}



