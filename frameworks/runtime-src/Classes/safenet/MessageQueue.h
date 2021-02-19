#pragma once

#include "Singleton.h"

#include "string"
#include <map>
#include <vector>
#include <functional>
#include "PriorityCycleQueue.h"

using namespace std;

class CMessageReceiver;
class CMessageSender;

typedef function<bool(void*,void*,int)> CQueueCall;

#define QUEUE_CALLBACK_3(func, _Object) bind(&func,_Object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

typedef  map<string,CQueueCall> ClassQueueReceiveMap;

class CMessageReceiver
{
public:			
	virtual bool onMessage(CMessageSender* sender,WorldPacket* packet,int msg_id)=0;
	string m_ReceiverName;
};

typedef vector<CMessageReceiver*> MessageReceiverList;

//WorldPacket发送的数据,含m_trans_id和m_trans_order，请求时m_trans_order表示对方需要返回的数据条数
typedef map<WorldPacket*,CMessageReceiver*> TransMessageList;//事务消息列表

class CMessageSender
{
public:	
	TransMessageList m_TransMessageList;
	string m_SenderName;
};

typedef map<time_t,CMessageSender*> MessageSenderMap;

class CMsgProcBase:public CMessageReceiver,public CMessageSender
{
public:
	CMsgProcBase();
	~CMsgProcBase();
    virtual string getClassName();//获得类名
};

class CQueueObject
{
public:
	string m_QueueName;

	MessageSenderMap m_TransMessageSenders;//transition，事务处理
	
	mutex m_recvs_queue_mtx;
	MessageReceiverList m_MessageReceivers;

	mutex m_data_queue_mtx;
	PriorityCycleQueue m_DataQueue;

	mutex m_class_queue_mtx;
	ClassQueueReceiveMap m_ClassReceivers;
public:
	bool isSenderInList(CMessageSender* sender,MessageSenderMap* map);
	bool isReceiverInList(CMessageReceiver * receiver,MessageReceiverList* list);

	bool removeSender(CMessageSender* sender,MessageSenderMap* map);
	bool removeReceiver(CMessageReceiver * receiver,MessageReceiverList* list);

	bool removeReveiver(string name);
};

typedef vector<CQueueObject*> MessageQueueList;

enum QueueNames
{
	qn_All=0
	,qn_net_send
	,qn_net_recv
	,qn_ecnomy_send
	,qn_ecnomy_recv
	,qn_ui_send
	,qn_ui_recv
	,qn_gamelogic_send
	,qn_gamelogic_recv
	,qn_battle_send
	,qn_battle_recv
};

//////////////////////////////////////////////////////////////////////////
//	queue url 定义
//	网络  inproc://net_send
//	网络  inproc://net_recv
//  经济  inproc://ecnomy_send
//  经济  inproc://ecnomy_recv
//  UI    inproc://ui_send
//  UI    inproc://ui_recv
//  逻辑  inproc://gamelogic_send
//  逻辑  inproc://gamelogic_recv
//  战斗  inproc://battle_send
//  战斗  inproc://battle_recv
//////////////////////////////////////////////////////////////////////////

enum QueueType
{
	qtRender=0
	,qtThread
};

class CMessageQueue: public Singleton<CMessageQueue>,public Node
{
public:	
	CMessageQueue();
	~CMessageQueue();

	//simple func , exists string not compatable
	bool subcribeQueue(string &receiver_name,CQueueCall call,QueueNames queue_name);
	bool removeSubcribe(string &receiver_name,QueueNames queue_name);
	bool sendMessage(string &sender_name,QueueNames queue_name,WorldPacket* data);

	//complex func, dependence inherit
	bool subcribeQueue(CMessageReceiver* receiver,QueueNames queue_name);
	bool removeSubcribe(CMessageReceiver* receiver,QueueNames queue_name);
	bool sendMessage(CMessageSender* sender,QueueNames queue_name,WorldPacket* data,bool useTrans=false);

	//complex string func, dependence inherit
	bool subcribeQueue(CMessageReceiver* receiver,string queue_url);
	bool removeSubcribe(CMessageReceiver* receiver,string queue_url);
	bool sendMessage(CMessageSender* sender,string queue_url,WorldPacket* data,bool useTrans=false);

	string getQueueName(QueueNames queue_enum);

protected:
	void setQueueType(QueueType qt=qtThread);
private:		

	MessageQueueList m_Queues;
	QueueType m_QueueType;
	bool m_IsClose;
	thread m_QueueThread;

	CQueueObject* getQueue(string qname);
	bool initQueues();
	bool clearQueues();

	void RenderFunc(float delta);
	void ThreadFunc(void* param);

	void DispatchData();
};


