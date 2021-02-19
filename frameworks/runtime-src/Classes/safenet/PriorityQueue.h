/******************************************************************************** 
 * create:cclin
 * date:2014-11-25
 * ver:
 * latest update:
 * function:优先队列,缓冲包结构
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;

#define qMax_Priority 1024
#define qMin_Priority 1

class WorldPacket
{
public:
	inline WorldPacket()
	{
		m_buffer = 0;
		m_size = 0;
		m_priority = 5;
		m_start_pos = 0;
		m_failcount = 0;
        m_tag = 0;
        m_command = 0;		
		m_create_time = time(0);
		m_trans_id = m_create_time;
		m_isdead = false;
		m_userpointer = 0;
	}

	inline WorldPacket(void* data,unsigned int s)
	{
		m_size = s;
		m_priority = 5;
		m_create_time = time(0);
		m_buffer = malloc(s);
		m_start_pos = 0;
		m_failcount = 0;
        m_tag = 0;
        m_command = 0;
		m_trans_id = m_create_time;
		m_trans_order = 0;
		memcpy(m_buffer,data,s);
		m_isdead = false;
		m_userpointer = 0;
	}

	inline WorldPacket(WorldPacket & packet)
	{
		m_size = packet.m_size;
		m_priority = packet.m_priority;
		m_create_time = time(0);
		m_buffer = malloc(m_size);
		m_start_pos = packet.m_start_pos;
		m_failcount = packet.m_failcount;
        m_tag = packet.m_tag;
        m_command = packet.m_command;
		m_trans_id = packet.m_trans_id;
		m_trans_order = packet.m_trans_order;
		memcpy(m_buffer,packet.m_buffer,m_size);
		m_isdead = packet.m_isdead;
		m_userpointer = packet.m_userpointer;
	}

	~WorldPacket()
	{
		m_size = 0;
		if(m_buffer!=0)
            free(m_buffer);
	}
	unsigned int* contents(){return (unsigned int*)m_buffer+m_start_pos;};
	unsigned int  size(){return m_size-m_start_pos;};
	unsigned int  allsize(){return m_size;};

	time_t m_create_time;
	int m_priority;
	int m_tag;    //reserve
    int m_command;//command < 101:data, 998 connect error,999:close current connect ;other:reserve
	unsigned int m_start_pos;
	unsigned int m_failcount;

	time_t m_trans_id;//trans, use time default
	int m_trans_order;//trans, order请求时m_trans_order表示对方需要返回的数据条数，回馈时为回馈的编号
	bool m_isdead;
	void* m_userpointer;

	inline WorldPacket* clone()
	{
		WorldPacket* wp=new WorldPacket(*this);		
		return wp;
	}

	inline void copy(void* data,unsigned int s)
	{
		if(m_buffer!=0)
			free(m_buffer);
		m_buffer=malloc(s);
		memcpy(m_buffer,data,s);
		m_size = s;
		m_start_pos = 0;
		m_priority = 5;
		m_failcount = 0;
        m_tag = 0;
        m_command = 0;
		m_create_time = time(0);
		m_trans_id = m_create_time;
		m_trans_order = 0;
		m_isdead = false;
		m_userpointer = 0;
	}

	inline void copy(WorldPacket* wp)
	{
		if(m_buffer!=0)
			free(m_buffer);
		m_buffer=malloc(wp->allsize());
		memcpy(m_buffer,wp->contents(),wp->allsize());
		m_size = wp->allsize();
		m_start_pos = wp->m_start_pos;
		m_priority = wp->m_priority;
		m_failcount = wp->m_failcount;
		m_create_time = wp->m_create_time;
        m_tag = wp->m_tag;
        m_command = wp->m_command;
		m_trans_id = wp->m_trans_id;
		m_trans_order = wp->m_trans_order;
		m_isdead = wp->m_isdead;
		m_userpointer = wp->m_userpointer;
	}

private:
	void * m_buffer;
	size_t m_size;
};

struct WorldPacketCmp  
{  
	bool operator()(const WorldPacket *na, const WorldPacket *nb)  
	{  
		if (na->m_priority != nb->m_priority)  
			return na->m_priority <= nb->m_priority; //bigger first
		else  
			return na->m_create_time > nb->m_create_time;//time little first
	}	
};

typedef priority_queue<WorldPacket*, vector<WorldPacket*>, WorldPacketCmp> Priority_Queue;