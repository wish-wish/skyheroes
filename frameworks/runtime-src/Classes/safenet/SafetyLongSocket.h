/******************************************************************************** 
 * create:cclin
 * date:2014-11-26
 * ver:
 * latest update:
 * function:³¤Á¬½Ó
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "SafetySocket.h"

extern const unsigned int BadCountTimes;
extern const unsigned int RearCountTimes;
extern const unsigned int ProcessRecvCount;
extern const unsigned int ProcessSendCount;

class SafetyLongSocket
{
public:
	explicit SafetyLongSocket(string ip,int port,string name,int tag=0);
	~SafetyLongSocket();

	bool SendDataInstant(void* data,int size);
	bool SendData(void* data,int size);
	bool SendData(WorldPacket* wp);

	void EnterDataFunc(sckDataFunc func);
	void ExitDataFunc();
    
	void EnterConnFunc(sckConnectFunc func);
	void ExitConnFunc();

	void CloseSocket();
    
    void Reconnect();
	
	void RenderUpdate(float delta);
    
    bool ProcessRecv();
    bool ProcessSend();
    
	string &GetIP(){return m_Addr.nServerIp;}
	int GetPort(){return m_Addr.nPort;}

	unsigned int GetHandle();
	string &getName(){return m_Addr.nName;}
	SocketAddress* getAddress(){return &m_Addr;}
    SafetySocket* getSocket(){return m_Socket;}
    
    bool checkSocket();
private:	
	
    SocketAddress m_Addr;
	SafetySocket* m_Socket;

	mutex m_recv_queue_mtx; 
	PriorityCycleQueue m_RecvQueue;

	mutex m_send_queue_mtx;
	PriorityCycleQueue m_SendQueue;

	PriorityCycleQueue m_BadQueue;

	bool connect(SocketAddress *pAddress);
	
    
	void destory();
	void DispatchData();
    
};