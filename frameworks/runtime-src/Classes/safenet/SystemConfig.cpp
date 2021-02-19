
#include "SystemConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#ifdef WIN32
#else
#include "unistd.h"
#endif

USING_NS_CC_EXT;
using namespace rapidjson;

SystemConfig::SystemConfig()
{
    this->retain();
    
    if(CCFileUtils::getInstance()->isFileExist(ConfigFile))
    {
        string json_data=CCFileUtils::getInstance()->getStringFromFile(ConfigFile);
        rapidjson::Document doc;
        if(json_data.compare("")!=0)
        {
            doc.Parse<kParseDefaultFlags>(json_data.c_str());
            rapidjson::Value&gateserver=doc["gateserver"];
            m_GatewayIp   = gateserver["serverip"].GetString();
            m_GatewayPort = gateserver["serverport"].GetInt();
            
            rapidjson::Value&loginserver=doc["loginerver"];
            m_LoginIp   = loginserver["serverip"].GetString();
            m_LoginPort = loginserver["serverport"].GetInt();
            
            rapidjson::Value&sound=doc["sound"];
			m_isBackgroundMusic = sound["backgroundmusic"].GetBool();
			m_isButtonSound = sound["buttonsound"].GetBool();
        }
    }
}

SystemConfig::~SystemConfig()
{
    this->release();
}

string SystemConfig::getGatewayIp()
{
    return m_GatewayIp;
}

int    SystemConfig::getGatewayPort()
{
    return m_GatewayPort;
}

string SystemConfig::getLoginIp()
{
    return m_LoginIp;
}

int    SystemConfig::getLoginPort()
{
    return m_LoginPort;
}

bool  SystemConfig::getIsBackgroundMusic()
{
    return m_isBackgroundMusic;
}

bool  SystemConfig::getIsButtonSound()
{
    return m_isButtonSound;
}


