/******************************************************************************** 
 * create:cclin
 * date:2014-11-25
 * ver:
 * latest update:
 * function:套接字
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include <functional>
#include "PriorityCycleQueue.h"
#include "cocos2d.h"

//#include <thread>         // std::thread
//#include <mutex>          // std::mutex

using namespace std;
USING_NS_CC;

#ifdef WIN32
	#include <windows.h>
	#include <WinSock.h>
	#include <process.h>
	#pragma comment( lib, "ws2_32.lib" )
	#pragma comment( lib, "libzlib.lib" )
#else
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #define SOCKET int
	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1
#endif

#ifdef PTHREAD
#include <pthread.h>
#else
#endif // PTHREAD

#define _MAX_MSGSIZE    (32*1024)		// 暂定一个消息最大为32k
#define _BLOCKSECONDS	30			// INIT函数阻塞时间
#define _INBUFSIZE	(256*1024)		//?	recv
#define _OUTBUFSIZE	(256*1024)		//? sendbuf，当不超过256K时，FLUSH只需要SEND一次

#define PINGINTERVAL_TIMER	15

enum ConnectStatus
{
	csConnUnkown
	,csConnecting
    ,csConnSuccess
	,csConnFail
	,csConnError
	,csConnClose
    ,csAutoReconnect
	,csDisconnect
    ,csObjectDestory
    ,csFailDialog
    ,csReconnDialog
};

enum ConnectControl
{
    ccUnknown
    ,ccOpen
    ,ccClose
};

struct SocketAddress;

typedef function<bool(SocketAddress*,WorldPacket*)> sckDataFunc;	
typedef function<void(SocketAddress*,ConnectStatus)> sckConnectFunc;	

struct SocketAddress
{
	string nName;
	string nServerIp;
	string nServerName;
	int nPort;
	int nTag;
	int nState;//0 no create,1 create 2 delete
	int nReConnectCount;
	int nErrorCode;
	unsigned int nHandle;
    
	PriorityCycleQueue m_SendQueue;
	time_t m_CloseTime;
	time_t m_LastConnectTime;
	time_t m_sendping_last_time;
    
    sckDataFunc m_DataFunc;
    sckConnectFunc m_ConnectFunc;
    
    ConnectControl m_UserControl;
    ConnectStatus  m_ConnStatus;
    
	SocketAddress()
	{
		nName="";
		nServerIp = "";
		nPort = 0;
		nTag = 0;
		nReConnectCount = 0;
		nErrorCode=0;
		nState = 0;
		nHandle = INVALID_SOCKET;
		m_SendQueue.empty();		
		m_CloseTime = time(0);
		m_LastConnectTime = time(0);
		m_sendping_last_time = time(0);
        m_DataFunc = nullptr;
        m_ConnectFunc = nullptr;
	}

	static int MaxTag;
	static int getMaxTag()
	{
		MaxTag++;
		return MaxTag;
	}
};

// 说明：func函数名, _Object指针, std::placeholders::_1, std::placeholders::_2表示回调函数的三个参数
#define SCT_CALLBACK_2(func, _Object) bind(&func,_Object, placeholders::_1, placeholders::_2)
#define SCT_CALLBACK_1(func, _Object) bind(&func,_Object, placeholders::_1)

class  SafetySocket
{
public:
	SafetySocket(void);
	~SafetySocket();
	virtual bool	Create( SocketAddress *saddr, int nBlockSec = _BLOCKSECONDS, bool bKeepAlive = false );
	virtual bool	SendMsg( void* pBuf, int nSize );	
	virtual bool	ReceiveMsg(void* pBuf, int& nSize);
	bool	Flush(void);
	bool	Check(void);
	void	Destroy(void);
	SOCKET	GetSocket(void) const { return m_sockClient; };
	
	int		getTagID(){ return m_sAddr->nTag; };
	string  getIP(){return m_sAddr->nServerIp;};
	int		getPort(){return m_sAddr->nPort;};
	SocketAddress * getAddress(){return m_sAddr;};
    
	bool    getIsDestory();
    virtual int getPacketLength();
protected:
	bool    m_isdestory;
	bool	recvFromSock(void);		// 从网络中读取尽可能多的数据
	bool    hasError();			// 是否发生错误，注意，异步模式未完成非错误
	
	void    closeSocket();
    

	SOCKET	m_sockClient;

	// 发送数据缓冲
	char	m_bufOutput[_OUTBUFSIZE];	//? 可优化为指针数组
	int		m_nOutbufLen;

	// 环形缓冲区
	char	m_bufInput[_INBUFSIZE];
	int		m_nInbufLen;
	int		m_nInbufStart;				// INBUF使用循环式队列，该变量为队列起点，0 - (SIZE-1)
	
	SocketAddress *m_sAddr;		
};

class SafetyQueueSocket:public SafetySocket
{
public:
    SafetyQueueSocket();
    ~SafetyQueueSocket();
    bool	addToQueue(WorldPacket* packet);
    bool	processQueue(int count=1);
protected:
    bool	FlushPacket(WorldPacket *packet);
};

class SafetySocket_wanghu:public SafetySocket
{
public:
    SafetySocket_wanghu();
    ~SafetySocket_wanghu();
	virtual bool	Create(SocketAddress *saddr, int nBlockSec = _BLOCKSECONDS, bool bKeepAlive = false);
	virtual bool SendMsg(void* pBuf, int nSize);
	virtual bool ReceiveMsg(void* pBuf, int& nSize);

	WorldPacket* newData(int tag, WORD vMain, WORD vSub, void* buf, int size);
	WorldPacket* addMac(WorldPacket* wp);

	virtual bool SendData(WORD wMain, WORD wSub, void* pBuf, int nSize);
    virtual int getPacketLength();
};

class SafetySocket_yxws:public SafetySocket
{
public:
    SafetySocket_yxws(){};
    ~SafetySocket_yxws(){};
    virtual int getPacketLength();
};

class SafetySocket_firefly:public SafetySocket
{
public:
    SafetySocket_firefly(){};
    ~SafetySocket_firefly(){};
    virtual int getPacketLength();
};

class SafetySocket_scut:public SafetySocket
{
public:
    SafetySocket_scut(){};
    ~SafetySocket_scut(){};
    virtual int getPacketLength();
};

class SafetySocket_kbe:public SafetySocket
{
public:
    SafetySocket_kbe(){};
    ~SafetySocket_kbe(){};
    virtual int getPacketLength();
};

class SafetySocket_promelo:public SafetySocket
{
public:
    SafetySocket_promelo(){};
    ~SafetySocket_promelo(){};
    virtual int getPacketLength();
};

class SafetySocket_erlang:public SafetySocket
{
public:
    SafetySocket_erlang(){};
    ~SafetySocket_erlang(){};
    virtual int getPacketLength();
};

class SafetySocket_trinitycore :public SafetySocket
{
public:
	SafetySocket_trinitycore(){};
	~SafetySocket_trinitycore(){};
	virtual int getPacketLength();
};



