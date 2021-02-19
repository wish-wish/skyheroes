/******************************************************************************** 
 * create:cclin
 * date:2014-11-24
 * ver:
 * latest update:
 * function:循环优先队列
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "PriorityQueue.h"

class NotifyDelegate{
public:
    virtual void onNetNotify(int msgid ,WorldPacket* p) = 0;
    virtual void onConnectFailed(int netstate, int errorcode) = 0;
    virtual void onConnectSuccess()=0;
};

class PriorityCycleQueue{
public:
	PriorityCycleQueue();
	~PriorityCycleQueue();
public:
	WorldPacket* top();
	void push(WorldPacket* wp);
	void pop();
	unsigned int size();
	void pop_push();
	bool empty();

	void add(WorldPacket* wp);
	WorldPacket* next();
private:
	Priority_Queue m_PriorQueue;
};