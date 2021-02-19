/******************************************************************************** 
 * create:cclin
 * date:2014-11-26
 * ver:
 * latest update:
 * function:∂Ã¡¨Ω”
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "SafetySocket.h"

extern const unsigned int ProcessRecvCount;

class SafetyShortSocket
{
public:
	explicit SafetyShortSocket(string ip,int port,string name,int tag=0);
	~SafetyShortSocket();

	bool SendData(void* data,int size);

	void EnterDataFunc(sckDataFunc func);
	void ExitDataFunc();
	void EnterConnFunc(sckConnectFunc func);
	void ExitConnFunc();

	void CloseSocket();
	
	void RenderUpdate(float delta);
	string &GetIP(){return m_Addr.nServerIp;}
	int GetPort(){return m_Addr.nPort;}

	unsigned int GetHandle();
	string &getName(){return m_Addr.nName;}
	SocketAddress* getAddress(){return &m_Addr;}
    SafetySocket* getSocket(){return m_Socket;}
protected:
	SocketAddress m_Addr;
	bool m_Close;
	SafetySocket_firefly* m_Socket;
	mutex m_recv_queue_mtx; 
	PriorityCycleQueue m_RecvQueue;
	bool connect(SocketAddress *pAddress);
	bool ProcessRecv();
	void destory();
	void DispatchData();	
	void recvthread(void* sss);
    bool m_isThreadStart;
};