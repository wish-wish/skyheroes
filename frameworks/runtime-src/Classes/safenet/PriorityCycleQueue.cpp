/******************************************************************************** 
 * create:cclin
 * date:2014-11-24
 * ver:
 * latest update:
 * function:优先循环队列
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "PriorityCycleQueue.h"


PriorityCycleQueue::PriorityCycleQueue()
{

}

PriorityCycleQueue::~PriorityCycleQueue()
{

}

WorldPacket* PriorityCycleQueue::top()
{
	return m_PriorQueue.top();
}

void PriorityCycleQueue::push(WorldPacket* wp)
{
	m_PriorQueue.push(wp);
}

void PriorityCycleQueue::pop()
{
	WorldPacket* p=m_PriorQueue.top();
	m_PriorQueue.pop();
	delete p;
}

unsigned int PriorityCycleQueue::size()
{
	return m_PriorQueue.size();
}

void PriorityCycleQueue::pop_push()
{
	WorldPacket* p=m_PriorQueue.top();
	m_PriorQueue.pop();
	p->m_create_time = time(0);
	m_PriorQueue.push(p);
}

bool PriorityCycleQueue::empty()
{
	return m_PriorQueue.empty();
}

void PriorityCycleQueue::add(WorldPacket* wp)
{
	this->push(wp);
}

WorldPacket* PriorityCycleQueue::next()
{
	return m_PriorQueue.top();
}
