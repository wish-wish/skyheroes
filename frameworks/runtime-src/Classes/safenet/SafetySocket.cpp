/******************************************************************************** 
 * create:cclin
 * date:2014-11-25
 * ver:
 * latest update:
 * function:socket
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "SafetySocket.h"
#include "WHEncrypt.h"
#include "WHHead.h"
#include "PacketAide.h"
#include "Struct.h"
#include "CMD_LogonServer.h"
#include "CMD_GameServer.h"

#ifdef PTHREAD
pthread_mutex_t m_mutex_addr;
pthread_mutex_t m_mutex_socket;
pthread_mutex_t m_mutex_queue;
#else
mutex m_mutex_addr;
mutex m_mutex_socket;
mutex m_mutex_queue;
#endif // PTHREAD

int SocketAddress::MaxTag=0;

long long getCurrentTime()    
{    
	struct timeval tv;    
	gettimeofday(&tv,NULL);    
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;    
}

SafetySocket::SafetySocket()
{ 
	m_isdestory = false;
	memset(m_bufOutput, 0, sizeof(m_bufOutput));
	memset(m_bufInput, 0, sizeof(m_bufInput));

#ifdef PTHREAD
	pthread_mutex_init(&m_mutex_addr, NULL);
	pthread_mutex_init(&m_mutex_socket, NULL);	
	pthread_mutex_init(&m_mutex_queue,NULL);
#else
#endif
}

SafetySocket::~SafetySocket()
{
#ifdef PTHREAD
	pthread_mutex_destroy(&m_mutex_addr);
	pthread_mutex_destroy(&m_mutex_socket);
	pthread_mutex_destroy(&m_mutex_queue);
#else
#endif
	
    m_sAddr->m_ConnStatus = csObjectDestory;
    if(m_sAddr->m_ConnectFunc)
    {
        m_sAddr->m_ConnectFunc(m_sAddr,csObjectDestory);
    }
    m_sAddr->nHandle = -1;
	m_isdestory = true;
}

void SafetySocket::closeSocket()
{		
#ifdef PTHREAD
	pthread_mutex_lock(&m_mutex_addr);
#else
	m_mutex_addr.lock();
#endif
	m_sAddr->nState = 0;
    m_sAddr->nHandle = -1;
	if(m_sAddr->nReConnectCount<2)
		m_sAddr->m_CloseTime = time(0);	
#ifdef PTHREAD
	pthread_mutex_unlock(&m_mutex_addr);
#else
	m_mutex_addr.unlock();
#endif
	CCLOG("%d SafetySocket::closeSocket %s,%d",m_sAddr->nTag,__FILE__,__LINE__);
#ifdef WIN32
    closesocket(m_sockClient);
	//CCLog("SafetySocket::closeSocket WSACleanup");
    //WSACleanup();
#else
    close(m_sockClient);
#endif
	
}

bool SafetySocket::Create(SocketAddress *saddr, int nBlockSec, bool bKeepAlive /*= FALSE*/)
{
	m_sAddr = saddr;
	// ������
	if(m_sAddr->nPort == 0 || m_sAddr->nServerIp.length() > 15|| m_sAddr->nServerIp.length() < 8) {
		return false;
	}

// #ifdef WIN32
// 	WSADATA wsaData;
// 	WORD version = MAKEWORD(2, 0);
// 	int ret = WSAStartup(version, &wsaData);//win sock start up
// 	if (ret != 0) {
// 		CCLog("SafetySocket::Create WSAStartup %d",ret);
// 		return false;
// 	}
// #endif

	// �������׽���
	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(m_sockClient == INVALID_SOCKET) {
        m_sAddr->m_ConnStatus = csConnFail;
        if(m_sAddr->m_ConnectFunc)
        {
            m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
        }
        closeSocket();
		return false;
	}

	// ����SOCKETΪKEEPALIVE
	int	optval=1;
	if(bKeepAlive)
	{		
		if(setsockopt(m_sockClient, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
		{
            m_sAddr->m_ConnStatus = csConnFail;
            if(m_sAddr->m_ConnectFunc)
            {
                m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
            }
            closeSocket();
			return false;
		}
	}

	//����SENDBUF		
	//const unsigned long SNDBUF_SIZE = 64 * 1024;
	optval = _INBUFSIZE;
	if (::setsockopt(m_sockClient, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
	{
         m_sAddr->m_ConnStatus = csConnFail;
        if(m_sAddr->m_ConnectFunc)
        {
            m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
        }
		closeSocket();
		return false;
	}
	
	//����RECVBUG
	optval = _OUTBUFSIZE;
	if (setsockopt(m_sockClient, SOL_SOCKET, SO_RCVBUF, (char *) &optval, sizeof(optval)))
	{
        m_sAddr->m_ConnStatus = csConnFail;
        if(m_sAddr->m_ConnectFunc)
        {
            m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
        }
		closeSocket();
		return false;
	}

	int nRes=-1;
#ifdef WIN32
    unsigned long nMode = 1;
    nRes = ioctlsocket(m_sockClient, FIONBIO, &nMode);
#else
	// set block way
	nRes = fcntl(m_sockClient, F_SETFL, O_NONBLOCK);
#endif
	
	if (nRes == SOCKET_ERROR) {
        m_sAddr->m_ConnStatus = csConnFail;
        if(m_sAddr->m_ConnectFunc)
        {
            m_sAddr->m_ConnectFunc(m_sAddr,csConnError);
        }
		closeSocket();
		return false;
	}

	unsigned long serveraddr = inet_addr(m_sAddr->nServerIp.c_str());
	if(serveraddr == INADDR_NONE)	// ���IP��ַ��ʽ����
	{
		//closeSocket();
		//return false;
		hostent	* hp;
		hp = ::gethostbyname(m_sAddr->nServerIp.c_str());
		if(hp == 0)
		{
            m_sAddr->m_ConnStatus = csConnFail;
            if(m_sAddr->m_ConnectFunc)
            {
                m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
            }
			closeSocket();
			return false;
		}
		serveraddr = ::inet_addr(hp->h_addr_list[0]);	// �� h_addr���Զ�ȡ��һ������IP��
	}

	sockaddr_in	addr_in;
	memset((void *)&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(m_sAddr->nPort);
	addr_in.sin_addr.s_addr = serveraddr;
	
	if(connect(m_sockClient, (sockaddr *)&addr_in, sizeof(addr_in)) == SOCKET_ERROR) {
		if (hasError()) {
			CCLOG("%d SafetySocket::Create connect hasError %d %s %d",m_sAddr->nTag,SOCKET_ERROR,__FILE__,__LINE__);
            m_sAddr->m_ConnStatus = csConnError;
            if(m_sAddr->m_ConnectFunc)
            {
                m_sAddr->m_ConnectFunc(m_sAddr,csConnError);
            }
			closeSocket();
			return false;
		}
		else	// WSAWOLDBLOCK
		{
			timeval timeout;
			timeout.tv_sec	= nBlockSec;
			timeout.tv_usec	= 0;
			fd_set writeset, exceptset;
			FD_ZERO(&writeset);
			FD_ZERO(&exceptset);
			FD_SET(m_sockClient, &writeset);
			FD_SET(m_sockClient, &exceptset);

			int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
			if (ret == 0 || ret < 0) {
                m_sAddr->m_ConnStatus = csConnFail;
                if(m_sAddr->m_ConnectFunc)
                {
                    m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
                }
				closeSocket();
				return false;
			} else	// ret > 0
			{
				ret = FD_ISSET(m_sockClient, &exceptset);
				if(ret)		// or (!FD_ISSET(m_sockClient, &writeset)
				{
                    m_sAddr->m_ConnStatus = csConnFail;
                    if(m_sAddr->m_ConnectFunc)
                    {
                        m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
                    }
					closeSocket();
					return false;
				}else
				{
					if (!FD_ISSET(m_sockClient, &writeset))
					{
                        m_sAddr->m_ConnStatus = csConnFail;
                        if(m_sAddr->m_ConnectFunc)
                        {
                            m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
                        }
						closeSocket();
						return false;
					}
					else
					{
#ifndef WIN32
						int err;
						socklen_t len = sizeof(err);  
						int code = getsockopt(m_sockClient, SOL_SOCKET, SO_ERROR, &err, &len); 
						if (code < 0 || err)
						{
                            m_sAddr->m_ConnStatus = csConnFail;
                            if(m_sAddr->m_ConnectFunc)
                            {
                                m_sAddr->m_ConnectFunc(m_sAddr,csConnError);
                            }
							closeSocket();
							return false;
						}
#endif
		
					}
				}
			}
		}
	}

	m_nInbufLen		= 0;
	m_nInbufStart	= 0;
	m_nOutbufLen	= 0;

	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 500;
	setsockopt(m_sockClient, SOL_SOCKET, SO_LINGER, (const char*)&so_linger, sizeof(so_linger));
	m_sAddr->nHandle=m_sockClient;
    m_sAddr->m_ConnStatus = csConnSuccess;
    if(m_sAddr->m_ConnectFunc)
    {
        m_sAddr->m_ConnectFunc(m_sAddr,csConnSuccess);
    }
	return true;
}

//////////////////////////////////////////////////////////////////////
bool SafetySocket::SendMsg( void* pBuf, int nSize )
{
	if(pBuf == 0 || nSize <= 0) {
		return false;
	}
	if (m_sockClient == INVALID_SOCKET) {
		return false;
	}
	//���ͨѶ��Ϣ������
	int packsize = 0;
	packsize = nSize;
	//���BUF���
	if(m_nOutbufLen + nSize > _OUTBUFSIZE) {
		//��������OUTBUF�е����ݣ������OUTBUF��
		Flush();
		if(m_nOutbufLen + nSize > _OUTBUFSIZE) {
			// ������
			Destroy();
			return false;
		}
	}
	//������ӵ�BUFβ
	memcpy(m_bufOutput + m_nOutbufLen, pBuf, nSize);
	m_nOutbufLen += nSize;
	return true;
}

bool SafetySocket::ReceiveMsg( void* pBuf, int& nSize )
{
	//������
	if(pBuf == NULL || nSize <= 0) {
		return false;
	}
	
	if (m_sockClient == INVALID_SOCKET) {
		return false;
	}

	// ����Ƿ���һ����Ϣ(С��2���޷���ȡ����Ϣ����)
	if(m_nInbufLen < 2) {
		//  ���û������ɹ�  ����   ���û��������ֱ�ӷ���
		if(!recvFromSock() || m_nInbufLen < 2) {		// ���m_nInbufLen������
			return false;
		}
	}

    // ����Ҫ��������Ϣ�Ĵ�С��һ����Ϣ����СΪ������Ϣ�ĵ�һ��16�ֽڣ�����Ϊ���λ�����������Ҫ�ֿ�����
	
    int packsize=this->getPacketLength();

	// �����Ϣ���ߴ���� �ݶ����16k
	if (packsize <= 0 || packsize > _MAX_MSGSIZE) {
		m_nInbufLen = 0;		// ֱ�����INBUF
		m_nInbufStart = 0;
		return false;
	}

	// �����Ϣ�Ƿ�����(�����Ҫ��������Ϣ���ڴ�ʱ���������ݳ��ȣ���Ҫ�ٴ��������ʣ������)
	if (packsize > m_nInbufLen) {
		// ���û������ɹ�   ����    ��Ȼ�޷���ȡ�����������ݰ�  �򷵻أ�ֱ��ȡ��������
		if (!recvFromSock() || packsize > m_nInbufLen) {	// ���m_nInbufLen�Ѹ���
			return false;
		}
	}

	// ���Ƴ�һ����Ϣ
	if(m_nInbufStart + packsize > _INBUFSIZE) {
		// ���һ����Ϣ�лؾ�����������ڻ��λ�������ͷβ��
		// �ȿ������λ�����ĩβ������
		int copylen = _INBUFSIZE - m_nInbufStart;
		memcpy(pBuf, m_bufInput + m_nInbufStart, copylen);

		// �ٿ������λ�����ͷ����ʣ�ಿ��
		memcpy((unsigned char *)pBuf + copylen, m_bufInput, packsize - copylen);
		nSize = packsize;
	} else {
		// ��Ϣû�лؾ�����һ�ο�����ȥ
		memcpy(pBuf, m_bufInput + m_nInbufStart, packsize);
		nSize = packsize;
	}

	// ���¼��㻷�λ�����ͷ��λ��
	m_nInbufStart = (m_nInbufStart + packsize) % _INBUFSIZE;
	m_nInbufLen -= packsize;
	return	true;
}

bool SafetySocket::hasError()
{
#ifdef WIN32
	int err = WSAGetLastError();
	if(err != WSAEWOULDBLOCK) {
#else
	int err = errno;
	if(err != EINPROGRESS && err != EAGAIN) {
#endif
		CCLOG("%d SafetySocket::hasError %d %s %d",m_sAddr->nTag,err,__FILE__,__LINE__);
#ifdef PTHREAD
		pthread_mutex_lock(&m_mutex_addr);
#else
		m_mutex_addr.lock();
#endif
        m_sAddr->nErrorCode = err;
        m_sAddr->m_ConnStatus = csConnError;
#ifdef PTHREAD
		pthread_mutex_unlock(&m_mutex_addr);
#else
		m_mutex_addr.unlock();
#endif
        if(m_sAddr->m_ConnectFunc)
        {
            m_sAddr->m_ConnectFunc(m_sAddr,csConnFail);
        }
		return true;
	}	
	return false;
}

// �������ж�ȡ�����ܶ�����ݣ�ʵ����������������ݵĵط�
bool SafetySocket::recvFromSock(void)
{
	if (m_nInbufLen >= _INBUFSIZE || m_sockClient == INVALID_SOCKET) {
		return false;
	}

	// ���յ�һ������
	int	savelen, savepos;			// ����Ҫ����ĳ��Ⱥ�λ��
	if(m_nInbufStart + m_nInbufLen < _INBUFSIZE)	{	// INBUF�е�ʣ��ռ��л���
		savelen = _INBUFSIZE - (m_nInbufStart + m_nInbufLen);		// �󲿿ռ䳤�ȣ����������ݵĳ���
	} else {
		savelen = _INBUFSIZE - m_nInbufLen;
	}

	// ���������ݵ�ĩβ
	savepos = (m_nInbufStart + m_nInbufLen) % _INBUFSIZE;
	//CHECKF(savepos + savelen <= INBUFSIZE);
	int inlen = recv(m_sockClient, m_bufInput + savepos, savelen, 0);
	if(inlen > 0) {
#if _DEBUG
		char buff[16]={0};		
		if(inlen<16)
			memcpy(buff,m_bufOutput,inlen);
		else
			memcpy(buff,m_bufOutput,16);				
		CCLOG("SafetySocket::recvFromSock %d First recv %d buff %s",m_sAddr->nTag,inlen,buff);
#endif		
		// �н��յ�����
		m_nInbufLen += inlen;
		
		if (m_nInbufLen > _INBUFSIZE) {
			return false;
		}

		// ���յڶ�������(һ�ν���û����ɣ����յڶ�������)
		if(inlen == savelen && m_nInbufLen < _INBUFSIZE) {
			int savelen = _INBUFSIZE - m_nInbufLen;
			int savepos = (m_nInbufStart + m_nInbufLen) % _INBUFSIZE;
			//CHECKF(savepos + savelen <= INBUFSIZE);
			inlen = recv(m_sockClient, m_bufInput + savepos, savelen, 0);	
			if(inlen > 0) {
#if _DEBUG
				char buff[16]={0};		
				if(inlen<16)
					memcpy(buff,m_bufOutput,inlen);
				else
					memcpy(buff,m_bufOutput,16);				
				CCLOG("SafetySocket::recvFromSock %d Second recv %d buff %s",m_sAddr->nTag,inlen,buff);
#endif	
				m_nInbufLen += inlen;
				if (m_nInbufLen > _INBUFSIZE) {
					return false;
				}	
			} else if(inlen == 0) {
				Destroy();
				return false;
			} else {
				// �����ѶϿ����ߴ��󣨰���������
				if (hasError()) {
					CCLOG("%d SafetySocket::recvFromSock recv has Error %d %s %d",m_sAddr->nTag,inlen,__FILE__,__LINE__);
					Destroy();
					return false;
				}
			}
		}
	} else if(inlen == 0) {
		Destroy();
		return false;
	} else {
		// �����ѶϿ����ߴ��󣨰���������
		if (hasError()) {
			CCLOG("%d SafetySocket::recvFromSock recv has Error %d %s %d",m_sAddr->nTag,inlen,__FILE__,__LINE__);
			Destroy();
			return false;
		}
	}

	return true;
}

bool SafetySocket::Flush(void)		//? ��� OUTBUF > SENDBUF ����Ҫ���SEND����
{
	if (m_sockClient == INVALID_SOCKET) {
		return false;
	}
	if(m_nOutbufLen <= 0) {
		return true;
	}	
	// ����һ������
	int	outsize=0;
	outsize = send(m_sockClient, m_bufOutput, m_nOutbufLen, 0);
	if(outsize > 0) {
#if _DEBUG
		char	buff[16]={0};		
		if(outsize<16)
			memcpy(buff,m_bufOutput,outsize);
		else
			memcpy(buff,m_bufOutput,16);				
		CCLOG("SafetySocket::Flush %d Send %d buff %s",m_sAddr->nTag,outsize,buff);
#endif
		// ɾ���ѷ��͵Ĳ���
		if(m_nOutbufLen - outsize > 0) {
			memcpy(m_bufOutput, m_bufOutput + outsize, m_nOutbufLen - outsize);
		}
		m_nOutbufLen -= outsize;

		if (m_nOutbufLen < 0) {
			return false;
		}
	} else {
		if (hasError()) {
			CCLOG("%d SafetySocket::Flush hasError %d %s %d",m_sAddr->nTag,outsize,__FILE__,__LINE__);
			Destroy();
			return false;
		}
	}
	return true;
}

bool SafetySocket::Check(void)
{
	// ���״̬
	if (m_sockClient == INVALID_SOCKET) {
		return false;
	}

	char buf[1];
// #ifdef WIN32
// 	Sleep(1000);
// #else
// 	usleep(1000*1000);
// #endif
	int	ret = recv(m_sockClient, buf, 1, MSG_PEEK);
	if(ret == 0) {
		Destroy();
		return false;
	} else if(ret < 0) {
		if (hasError()) {
			CCLOG("%d SafetySocket::Check hasError %d %s %d",m_sAddr->nTag,ret,__FILE__,__LINE__);
			Destroy();
			return false;
		} else {	// ����
			return true;
		}
	} else {	// ������
		return true;
	}
	
	return true;
}

void SafetySocket::Destroy(void)
{
	if(m_isdestory)
		return;
	// �ر�
	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 500;
	int ret = setsockopt(m_sockClient, SOL_SOCKET, SO_LINGER, (const char*)&so_linger, sizeof(so_linger));
    CCLOG("%d SafetySocket::Destroy ret %d %s %d",m_sAddr->nTag,ret,__FILE__,__LINE__);
    m_sAddr->m_ConnStatus = csConnClose;
    if(m_sAddr->m_ConnectFunc)
    {
        m_sAddr->m_ConnectFunc(m_sAddr,csConnClose);
    }
    closeSocket();

	m_sockClient = INVALID_SOCKET;
	m_nInbufLen = 0;
	m_nInbufStart = 0;
	m_nOutbufLen = 0;

	memset(m_bufOutput, 0, sizeof(m_bufOutput));
	memset(m_bufInput, 0, sizeof(m_bufInput));
}

bool SafetySocket::getIsDestory()
{
	return m_isdestory;
}
    
int SafetySocket::getPacketLength()
{
    int packsize = (unsigned char)m_bufInput[m_nInbufStart] + (unsigned char)m_bufInput[(m_nInbufStart + 1) % _INBUFSIZE] * 256;
    return packsize;
}
    
SafetyQueueSocket::SafetyQueueSocket()
{
    
}

SafetyQueueSocket::~SafetyQueueSocket()
{
    
}

bool SafetyQueueSocket::addToQueue(WorldPacket* packet)
{
#ifdef PTHREAD
	pthread_mutex_lock(&m_mutex_queue);
#else
	m_mutex_queue.lock();
#endif
    WorldPacket* np=packet->clone();
#ifdef PTHREAD
	pthread_mutex_unlock(&m_mutex_addr);
#else
	m_mutex_addr.unlock();
#endif
    return true;
}

bool SafetyQueueSocket::processQueue(int count)
{
    bool ret=false;
    unsigned int size=count;
    if(size>m_sAddr->m_SendQueue.size())
        size = m_sAddr->m_SendQueue.size();
    for(unsigned int i=0;i<size;i++)
    {
        WorldPacket* packet = m_sAddr->m_SendQueue.top();
        if((time(0) - packet->m_create_time >= PINGINTERVAL_TIMER)&&packet->m_priority==0)
        {	//ɾ�������������ȵ�������
#ifdef PTHREAD
			pthread_mutex_lock(&m_mutex_queue);
#else
			m_mutex_queue.lock();
#endif
            m_sAddr->m_SendQueue.pop();
#ifdef PTHREAD
			pthread_mutex_unlock(&m_mutex_queue);
#else
			m_mutex_queue.unlock();
#endif
            delete packet;
            continue;
        }
        ret = this->FlushPacket(packet);
        if(ret)
        {
#ifdef PTHREAD
			pthread_mutex_lock(&m_mutex_queue);
#else
			m_mutex_queue.lock();
#endif
            m_sAddr->m_SendQueue.pop();
#ifdef PTHREAD
			pthread_mutex_unlock(&m_mutex_queue);
#else
			m_mutex_queue.unlock();
#endif
            delete packet;
        }		
    }	
    return ret;
}

bool SafetyQueueSocket::FlushPacket(WorldPacket *packet)
{
#ifdef PTHREAD
	pthread_mutex_lock(&m_mutex_socket);
#else
	m_mutex_socket.lock();
#endif
    bool flag1=this->SendMsg((void*)packet->contents(),packet->size());
    bool flag2 = this->Flush();
#ifdef PTHREAD
	pthread_mutex_unlock(&m_mutex_socket);
#else
	m_mutex_socket.unlock();
#endif
    return flag1&&flag2;
}

CEncrypt* gEncrypt = 0;
    
SafetySocket_wanghu::SafetySocket_wanghu()
{
	if (gEncrypt == 0)
	{
		gEncrypt = new CEncrypt();
	}
}

SafetySocket_wanghu::~SafetySocket_wanghu()
{

}

bool SafetySocket_wanghu::Create(SocketAddress *saddr, int nBlockSec /*= _BLOCKSECONDS*/, bool bKeepAlive /*= false*/)
{
	bool ret = SafetySocket::Create(saddr, nBlockSec, bKeepAlive);
	gEncrypt->addOrGetKey(this->getTagID());
	return ret;
}

WorldPacket* SafetySocket_wanghu::newData(int tag, WORD vMain, WORD vSub, void* buf, int size)
{
	if (gEncrypt == 0)
	{
		gEncrypt = new CEncrypt();
	}
	SocketKey* key = gEncrypt->addOrGetKey(tag);
	CMD_Head head;
	head.CmdInfo.cbVersion = SOCKET_VER;
	head.CmdInfo.cbCheckCode = 0;//key->m_cbSendRound;
	head.CmdInfo.wPacketSize = size+sizeof(CMD_Head);
	head.CommandInfo.wMainCmdID = vMain;
	head.CommandInfo.wSubCmdID = vSub;
	char szBuffer[_MAX_MSGSIZE] = { 0 };
	memcpy(szBuffer, (void*)&head, sizeof(CMD_Head));
	memcpy(szBuffer + sizeof(CMD_Head)+1, buf, size);
	WorldPacket* wp = new WorldPacket(szBuffer, size + sizeof(CMD_Head));
	return wp;
}

WorldPacket* SafetySocket_wanghu::addMac(WorldPacket* wp)
{
	
	tagClientSerial ClientSerial;
	//�������������ֽ��� 
	ClientSerial.dwSystemVer = 0x00020005;
	ClientSerial.dwComputerID[0] = 0x00;
	ClientSerial.dwComputerID[1] = 0x00;
	ClientSerial.dwComputerID[2] = 0x00;
	//��ӻ������к�
	char szBuffer[_MAX_MSGSIZE] = { 0 };
	memcpy(szBuffer, wp->contents(), wp->size());
	CSendPacketHelper Packet(szBuffer + sizeof(CMD_GP_RegisterAccounts), sizeof(szBuffer) - sizeof(CMD_GP_RegisterAccounts));
	Packet.AddPacket(&ClientSerial, sizeof(ClientSerial), 1000);

	delete wp;
	WorldPacket* retwp = new WorldPacket(szBuffer, sizeof(CMD_GP_RegisterAccounts) + sizeof(ClientSerial));

	return retwp;
}

bool SafetySocket_wanghu::SendMsg(void* pBuf, int nSize)
{
	BYTE cbDataBuffer[_MAX_MSGSIZE];
	memset(cbDataBuffer, 0, _MAX_MSGSIZE);
	memcpy(cbDataBuffer, pBuf, nSize);	
	WORD wSendSize=gEncrypt->EncryptBuffer(this->getTagID(), cbDataBuffer, nSize, _MAX_MSGSIZE);
	
	//CMD_Head* head = (CMD_Head*)cbDataBuffer;
	//head->CmdInfo.wPacketSize = wSendSize;

	bool ret=SafetySocket::SendMsg(cbDataBuffer, wSendSize);
	return ret;
}

bool SafetySocket_wanghu::ReceiveMsg(void* pBuf, int& nSize)
{
	bool ret = SafetySocket::ReceiveMsg(pBuf, nSize);
	if (ret)
	{
		BYTE cbDataBuffer[_MAX_MSGSIZE];
		memset(cbDataBuffer, 0, _MAX_MSGSIZE);
		memcpy(cbDataBuffer, pBuf, nSize);
		WORD wRecvSize = gEncrypt->DecryptBuffer(this->getTagID(), cbDataBuffer, nSize);
		memcpy(pBuf, cbDataBuffer, wRecvSize);
	}
	/*
	WORD wRealySize = ((CMD_Head *)cbDataBuffer)->CmdInfo.wPacketSize;
	WORD wDataSize = wRealySize - sizeof(CMD_Head);
	void * pDataBuffer = cbDataBuffer + sizeof(CMD_Head);//����ͷ����, ��Ϊҵ������.
	CMD_Command Command = ((CMD_Head *)cbDataBuffer)->CommandInfo;
	*/
	return ret;
}

bool SafetySocket_wanghu::SendData(WORD wMain, WORD wSub, void* pBuf, int nSize)
{
	BYTE cbDataBuffer[_MAX_MSGSIZE];
	memset(cbDataBuffer, 0, _MAX_MSGSIZE);
	CMD_Head * pHead = (CMD_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID = wMain;
	pHead->CommandInfo.wSubCmdID = wSub;
	WORD size = sizeof(CMD_Head) + nSize;
	pHead->CmdInfo.wPacketSize = size;
	pHead->CmdInfo.cbVersion = SOCKET_VER;
	SocketKey* key = gEncrypt->addOrGetKey(this->getTagID());
	pHead->CmdInfo.cbCheckCode = key->m_cbSendRound;
	if (nSize > 0)
	{
		assert(pBuf != NULL);
		memcpy(pHead + 1, pBuf, nSize);
	}
	bool ret=this->SendMsg(pBuf, size);
	return ret;
}

int SafetySocket_wanghu::getPacketLength()
{
    //wanghu server
    int packsize = (unsigned char)m_bufInput[m_nInbufStart+2] +(unsigned char)m_bufInput[(m_nInbufStart + 3) % _INBUFSIZE] * 256; // ע���ֽ��򣬸�λ+��λ
    return packsize;
}

int SafetySocket_yxws::getPacketLength()
{
    //yxws server
    int packsize = (unsigned char)m_bufInput[m_nInbufStart] + (unsigned char)m_bufInput[(m_nInbufStart + 1) % _INBUFSIZE] * 256; // ע���ֽ��򣬸�λ+��λ
    return packsize;
}

int SafetySocket_firefly::getPacketLength()
{
    //firefly server
    int packsize = 0;
    char* pos=(char*)&packsize;
    for(int i = 0;i < 4 ;i++)
    {
        pos[i] = m_bufInput[(m_nInbufStart+9+i) % _INBUFSIZE];
    }
    packsize+=17-4;
    return packsize;
}
    
int SafetySocket_scut::getPacketLength()
{
    //todo::
    int packsize = SafetySocket::getPacketLength();
    return packsize;
}

int SafetySocket_kbe::getPacketLength()
{
    //todo::
    int packsize = SafetySocket::getPacketLength();
    return packsize;
}
    
int SafetySocket_promelo::getPacketLength()
{
    //todo::
    int packsize = SafetySocket::getPacketLength();
    return packsize;
}

int SafetySocket_erlang::getPacketLength()
{
    //todo::
    int packsize = SafetySocket::getPacketLength();
    return packsize;
}

int SafetySocket_trinitycore::getPacketLength()
{
	//todo::wow server simulator
	int packsize = SafetySocket::getPacketLength();
	return packsize;
}
    
