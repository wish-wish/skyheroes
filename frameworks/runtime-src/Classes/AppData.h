#pragma once

#include "TileResolver.h"
#include "cocos2d.h"
#include "sunmap/Core/Singleton.h"

USING_NS_CC;

class AppSetting :public Singleton < AppSetting >
{
public:
	AppSetting();
	virtual ~AppSetting();
	void setLock(bool is){ m_lock = is; }
	bool getLock(){ return m_lock; }
	void setSelMode(int mode){ m_sel_mode = mode; }
	int getSelMode(){ return m_sel_mode; }
private:
	int m_sel_mode;
	bool m_lock;
};