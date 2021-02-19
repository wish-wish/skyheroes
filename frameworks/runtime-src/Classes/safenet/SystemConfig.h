
#pragma once

#include "Singleton.h"
#include "string"
#include <map>
#include <vector>
#include <functional>
#include "cocos2d.h"

using namespace std;
USING_NS_CC;

#define ConfigFile "res/net.json"

class SystemConfig: public Singleton<SystemConfig>,public Node
{
public:	
	SystemConfig();
	~SystemConfig();
    
    string getGatewayIp();
    int    getGatewayPort();
    string getLoginIp();
    int    getLoginPort();
    
    bool  getIsBackgroundMusic();
    bool  getIsButtonSound();
private:
    string m_GatewayIp;
    int    m_GatewayPort;
    string m_LoginIp;
    int    m_LoginPort;
    
    bool   m_isBackgroundMusic;
    bool   m_isButtonSound;
};


