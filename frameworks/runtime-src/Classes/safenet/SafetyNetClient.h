/******************************************************************************** 
 * create:cclin
 * date:2014-11-24
 * ver:
 * latest update:
 * function:安全网络，支持长连接，不支持短连接
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "CocosGUI.h"
#include "PriorityCycleQueue.h"
#include "SafetyShortSocket.h"
#include "SafetyLongSocket.h"
#include "SafetyHttpSocket.h"
#include "SafetyNetGuard.h"
#include "MessageQueue.h"

USING_NS_CC;
using namespace std;
using namespace ui;

extern const unsigned int BadCountTimes;
extern const unsigned int RearCountTimes;
extern const unsigned int ProcessSendCount;
extern const unsigned int ProcessRecvCount;

enum NetClientType
{
	nstRender=0
	,nstThread
};

enum SocketType
{
	stLong=0
	,stShort
};

typedef vector<SafetyShortSocket*> TShortSocketList;
typedef vector<SafetyLongSocket*> TLongSocketList;
typedef vector<SafetyNetGuard*> TNetGuardList;
typedef vector<SafetyHttpSocket*> THttpSocketList;

class SafetyNetClient :public Singleton<SafetyNetClient>, public Node, public CMsgProcBase
{
public:
	SafetyNetClient();
	~SafetyNetClient();

public:
	bool SendData(void* data,unsigned int s,string name="");//data不要free
	bool SendPacket(WorldPacket* wp,string name="");//wp需创建new，不要delete
	
	void Start(NetClientType nct=nstThread,int param=0);
	void Stop();			

	void EnterDataFunc(sckDataFunc func);
	void ExitDataFunc();

    SafetyShortSocket* NewShortSocket(string ip,int port,string name="");
    void DelShortSocket(string ip,int port);
    
    SafetyHttpSocket* NewHttpSocket(string ip,int port,string name="");
    
    SafetyLongSocket* NewLongSocket(string ip,int port,string name="");
    void DelLongSocket(string ip,int port);
    
    void DelSocket(int socket);
    
	SafetyLongSocket* getSocket(string name);
	SafetyLongSocket* getSocket(int tag);
    SafetyLongSocket* getSocket(string ip,int port);
	int getSocketTag(string ip,int port);
	int getMaxTag();
    string getSocketName(string ip,int port);

	void* PackNetfoxData(WORD vMain, WORD vSub,void* buf);
private:
	TShortSocketList m_ShortSocketList;
	TLongSocketList  m_LongSocketList;
    TNetGuardList    m_NetGuardList;
    THttpSocketList  m_HttpSocketList;
    
	bool m_isClose;
	bool m_isStarted;
	sckDataFunc m_DataFunc;
	NetClientType m_NetClientType;

	void ThreadFunc(void* param);
	void RenderFunc(float delta);

	void ProcessSend();
	void ProcessRecv();
	void StartThread(int param=0);	

	bool DataCall(SocketAddress* addr,WorldPacket* wp);
    
    void DelNetGuard(SafetyLongSocket *longsocket);
    void DelHttpSocket(SafetyShortSocket *shortsocket);
    
    //message queue
    virtual string getClassName(){return "SafetyNetClient";};
    bool onMessage(CMessageSender* sender,WorldPacket* packet,int msg_id);
};