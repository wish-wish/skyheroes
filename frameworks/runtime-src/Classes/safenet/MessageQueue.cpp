
#include "messagequeue.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#ifdef WIN32
#else
#include "unistd.h"
#endif

USING_NS_CC;
USING_NS_CC_EXT;

CMsgProcBase::CMsgProcBase()
{
	m_ReceiverName = m_SenderName = getClassName();
}

CMsgProcBase::~CMsgProcBase()
{

}

string CMsgProcBase::getClassName()
{
    return "CMsgProcBase";
}

bool CQueueObject::isSenderInList(CMessageSender* sender,MessageSenderMap* map)
{

	MessageSenderMap::iterator iter=map->begin();
	while(iter!=map->end())
	{
		if(iter->second==sender)
		{			
			return true;
		}
		iter++;
	}	
	return false;
}

bool CQueueObject::isReceiverInList(CMessageReceiver * receiver,MessageReceiverList* list)
{
	for(int i=0;i<list->size();i++)
	{
		if((*list)[i]==receiver)
			return true;
	}
	return false;
}		

bool CQueueObject::removeSender(CMessageSender* sender,MessageSenderMap* map)
{
	MessageSenderMap::iterator iter=map->begin();
	while(iter!=map->end())
	{
		if(iter->second==sender)
		{
			map->erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}

bool CQueueObject::removeReceiver(CMessageReceiver * receiver,MessageReceiverList* list)
{
	MessageReceiverList::iterator iter=list->begin();
	while(iter!=list->end())
	{
		if((*iter)==receiver)
		{
			list->erase(iter);
			return true;
		}
		iter++;
	}
	return false;
}

bool CQueueObject::removeReveiver(string name)
{
	ClassQueueReceiveMap::iterator iter=m_ClassReceivers.find(name);
	if(iter!=m_ClassReceivers.end())
	{
		m_ClassReceivers.erase(iter);
		return true;
	}
	return false;
}

CMessageQueue::CMessageQueue()
{
	this->initQueues();
	m_IsClose = false;
	this->retain();
	if(!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(CMessageQueue::RenderFunc),this))
		Director::getInstance()->getScheduler()->schedule(schedule_selector(CMessageQueue::RenderFunc),this,0.0f,false);
	m_QueueType = qtRender;
	m_QueueThread= thread(&CMessageQueue::ThreadFunc,this,(void*)0);
	m_QueueThread.detach();
}

CMessageQueue::~CMessageQueue()
{
	this->clearQueues();
	if(Director::getInstance()->getScheduler()->isScheduled(schedule_selector(CMessageQueue::RenderFunc),this))
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(CMessageQueue::RenderFunc),this);
	m_IsClose = true;	
	this->release();
}

string CMessageQueue::getQueueName(QueueNames queue_enum)
{
	string ret="";
	switch (queue_enum)
	{
		default:
	case qn_All:
		ret="inproc://all";
		break;
	case qn_net_send:
		ret="inproc://net_send";
		break;
	case qn_net_recv:
		ret="inproc://net_recv";
		break;
	case qn_ecnomy_send:
		ret="inproc://ecnomy_send";
		break;
	case qn_ecnomy_recv:
		ret="inproc://ecnomy_recv";
		break;
	case qn_ui_send:
		ret="inproc://ui_send";
		break;
	case qn_ui_recv:
		ret="inproc://ui_recv";
		break;
	case qn_gamelogic_send:
		ret="inproc://gamelogic_send";
		break;
	case qn_gamelogic_recv:
		ret="inproc://gamelogic_recv";
		break;
	case qn_battle_send:
		ret="inproc://battle_send";
		break;
	case qn_battle_recv:
		ret="inproc://battle_recv";
		break;
	}	
	return ret;
}

bool CMessageQueue::subcribeQueue(string &receiver_name,CQueueCall call,QueueNames queue_name)
{
	if(queue_name==qn_All)
	{
		for(int i=0;i<m_Queues.size();i++)
		{
			m_Queues[i]->m_class_queue_mtx.lock();
			m_Queues[i]->m_ClassReceivers[receiver_name]=call;
			m_Queues[i]->m_class_queue_mtx.unlock();
		}
		return true;
	}else
	{
		string qname=this->getQueueName(queue_name);
		CQueueObject* q=this->getQueue(qname);
		if(q==nullptr)
			return false;
		q->m_ClassReceivers[receiver_name]=call;
		return true;
	}	
}

bool CMessageQueue::removeSubcribe(string &receiver_name,QueueNames queue_name)
{
	if(queue_name==qn_All)
	{
		for(int i=0;i<m_Queues.size();i++)
		{
			m_Queues[i]->m_class_queue_mtx.lock();
			m_Queues[i]->removeReveiver(receiver_name);
			m_Queues[i]->m_class_queue_mtx.unlock();
		}
		return true;
	}else
	{
		string qname=this->getQueueName(queue_name);
		CQueueObject* q=this->getQueue(qname);
		if(q==nullptr)
			return false;
		q->removeReveiver(receiver_name);
		return true;
	}	
}


bool CMessageQueue::sendMessage(string &sender_name,QueueNames queue_name,WorldPacket* data)
{
	if(queue_name==qn_All)
	{
		data->m_userpointer=(void*)sender_name.c_str();
		for(int i=0;i<m_Queues.size();i++)
		{
			m_Queues[i]->m_data_queue_mtx.lock();
			m_Queues[i]->m_DataQueue.push(data->clone());
			m_Queues[i]->m_data_queue_mtx.unlock();
		}
		return true;
		delete data;
	}else
	{
		string qname=this->getQueueName(queue_name);
		CQueueObject* q=this->getQueue(qname);
		if(q==nullptr)
			return false;
		q->m_data_queue_mtx.lock();
		q->m_DataQueue.push(data);
		q->m_data_queue_mtx.unlock();
		return true;
	}	
}

bool CMessageQueue::subcribeQueue(CMessageReceiver* receiver,QueueNames queue_name)
{
	if(queue_name==qn_All)	
	{
		for(int i=0;i<m_Queues.size();i++)
		{
			if(!m_Queues[i]->isReceiverInList(receiver,&m_Queues[i]->m_MessageReceivers))
			{
				m_Queues[i]->m_recvs_queue_mtx.lock();
				m_Queues[i]->m_MessageReceivers.push_back(receiver);
				m_Queues[i]->m_recvs_queue_mtx.unlock();
			}
		}
		return true;
	}else
	{
		return  this->subcribeQueue(receiver,this->getQueueName(queue_name));
	}
}

bool CMessageQueue::subcribeQueue(CMessageReceiver* receiver,string queue_url)
{
	CQueueObject* q=this->getQueue(queue_url);
	if(q==nullptr)
		return false;
	if(!q->isReceiverInList(receiver,&q->m_MessageReceivers))
	{
		q->m_recvs_queue_mtx.lock();
		q->m_MessageReceivers.push_back(receiver);
		q->m_recvs_queue_mtx.unlock();
	}
	return true;
}

bool CMessageQueue::removeSubcribe(CMessageReceiver* receiver,QueueNames queue_name)
{
	if(queue_name==qn_All)	
	{
		for(int i=0;i<m_Queues.size();i++)
		{
			if(!m_Queues[i]->isReceiverInList(receiver,&m_Queues[i]->m_MessageReceivers))
			{
				m_Queues[i]->m_recvs_queue_mtx.lock();
				m_Queues[i]->m_MessageReceivers.push_back(receiver);
				m_Queues[i]->m_recvs_queue_mtx.unlock();
			}
		}
		return true;
	}else
	{
		return this->removeSubcribe(receiver,this->getQueueName(queue_name));
	}
}

bool CMessageQueue::removeSubcribe(CMessageReceiver* receiver, string queue_url)
{
	CQueueObject* q=this->getQueue(queue_url);
	if(q==nullptr)
		return false;
	if(q->isReceiverInList(receiver,&q->m_MessageReceivers))
	{
		q->m_recvs_queue_mtx.lock();
		q->removeReceiver(receiver,&q->m_MessageReceivers);
		q->m_recvs_queue_mtx.unlock();
	}
	return true;
}

bool CMessageQueue::initQueues()
{

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

	CQueueObject * obj=new CQueueObject();
	obj->m_QueueName = "inproc://net_send";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://net_recv";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://ecnomy_send";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://ecnomy_recv";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://ui_send";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://ui_recv";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://gamelogic_send";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://gamelogic_recv";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://battle_send";
	m_Queues.push_back(obj);
	obj=new CQueueObject();
	obj->m_QueueName = "inproc://battle_recv";
	m_Queues.push_back(obj);

	return true;
}

bool CMessageQueue::clearQueues()
{
	for(int i=0;i<m_Queues.size();i++)
	{
		delete m_Queues[i];
	}
	m_Queues.clear();

	return true;
}

void CMessageQueue::RenderFunc( float delta )
{
	if(m_QueueType==qtRender)
	{
		//dispatch data
		this->DispatchData();
	}
}

void CMessageQueue::ThreadFunc(void* param)
{
	while (!m_IsClose)
	{
		//dispatch data
#ifdef WIN32
		Sleep(20);
#else
		usleep(20*100);
#endif
		if(m_QueueType==qtThread)
		{
			this->DispatchData();
		}		
	}
}

void CMessageQueue::DispatchData()
{
	for(int i=0;i<m_Queues.size();i++)
	{
		CQueueObject* queue=m_Queues[i];
		for(int j=0;j<queue->m_DataQueue.size();j++)
		{				
			WorldPacket* wp=queue->m_DataQueue.top();
			if(wp->m_isdead)
			{
				queue->m_data_queue_mtx.lock();
				queue->m_DataQueue.pop();
				queue->m_data_queue_mtx.unlock();
			}			
			for(int k=0;k<queue->m_MessageReceivers.size();k++)
			{
				queue->m_MessageReceivers[i]->onMessage((CMessageSender*)wp->m_userpointer,wp,wp->m_command);
			}
			ClassQueueReceiveMap::iterator iter=queue->m_ClassReceivers.begin();
			while (iter!=queue->m_ClassReceivers.end())
			{
				if(iter->second!=nullptr)
				{
					iter->second(wp->m_userpointer,wp,wp->m_command);
				}
				iter++;
			}
			wp->m_isdead = true;
			wp->m_priority = qMax_Priority+1;
		}
	}
}


void CMessageQueue::setQueueType(QueueType qt)
{
	m_QueueType=qt;
}

CQueueObject* CMessageQueue::getQueue(string qname)
{
	for(int i=0;i<m_Queues.size();i++)
	{
		if(m_Queues[i]->m_QueueName.compare(qname)==0)
		{
			return m_Queues[i];
		}
	}
	return nullptr;
}

bool CMessageQueue::sendMessage(CMessageSender* sender,QueueNames queue_name,WorldPacket* data,bool useTrans)
{
	if(queue_name==qn_All)	
	{
		data->m_userpointer = (void*)sender;
		for(int i=0;i<m_Queues.size();i++)
		{
			m_Queues[i]->m_data_queue_mtx.lock();
			m_Queues[i]->m_DataQueue.push(data->clone());
			m_Queues[i]->m_data_queue_mtx.unlock();
		}
		return true;
		delete data;
	}else
	{
		return this->sendMessage(sender,this->getQueueName(queue_name),data,useTrans);
	}	
}

bool CMessageQueue::sendMessage(CMessageSender* sender,string queue_url,WorldPacket* data,bool useTrans)
{
	CQueueObject* q=this->getQueue(queue_url);
	if(q==nullptr)
		return false;
	data->m_userpointer = (void*)sender;
	q->m_data_queue_mtx.lock();
	q->m_DataQueue.push(data);
	q->m_data_queue_mtx.unlock();
	return true;
}