#include "AppDelegate.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "cocos2d.h"
#include "scripting/lua-bindings/manual/lua_module_register.h"
#include "scripting/lua-bindings/auto/lua_sky_rad_auto.hpp"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

#include "MapScene.h"
#include "PublicMsgHandler.h"
#include "base/CCScheduler.h"
#include "AppData.h"
#include "CMD_LogonServer.h"
#include "CMD_GameServer.h"
#include "SystemConfig.h"
extern "C"{
	extern int luaopen_pack(lua_State *L);
}
USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate()
	:m_type(0)
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif

#if (COCOS2D_DEBUG > 0) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
    // NOTE:Please don't remove this call if you want to debug with Cocos Code IDE
    RuntimeEngine::getInstance()->end();
#endif

}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages, 
// don't modify or remove this function
static int register_all_packages(lua_State* L)
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
	new AppSetting();

    // set default FPS
    Director::getInstance()->setAnimationInterval(1.0 / 60.0f);

    // register lua module
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* L = engine->getLuaStack()->getLuaState();
    lua_module_register(L);
	register_all_sky_rad(L);
	luaopen_pack(L);
    register_all_packages(L);

    LuaStack* stack = engine->getLuaStack();
    stack->setXXTEAKeyAndSign("2dxLua", strlen("2dxLua"), "XXTEA", strlen("XXTEA"));

    //register custom function
    //LuaStack* stack = engine->getLuaStack();
    //register_custom_function(stack->getLuaState());
    
#if CC_64BITS
    FileUtils::getInstance()->addSearchPath("src/64bit");
#endif
    FileUtils::getInstance()->addSearchPath("src");
    FileUtils::getInstance()->addSearchPath("res");
	FileUtils::getInstance()->addSearchPath("coin");

	if (this->m_type == 0)
	{
		PublicMsgHandler* pMsgHandler = new PublicMsgHandler();
		Scene *pScene = MapScene::scene(pMsgHandler);
		Director::getInstance()->runWithScene(pScene);
		Scheduler* schedular = Director::getInstance()->getScheduler();
		//schedular->schedule(CC_CALLBACK_0(AppDelegate::delayRun,this),this,0.03,false);
		schedular->scheduleUpdate(this, 0, false);
		//Director::getInstance()->setDisplayStats(false);
		isExecute = false;
		this->m_time = 0.0f;
	}
	else
	{
		auto engine = LuaEngine::getInstance();
		if (engine->executeScriptFile("main.lua"))
		{
			CCLOG("error execute main.lua");
		}
	}

	string ip = SystemConfig::getInstance()->getLoginIp();
	int port = SystemConfig::getInstance()->getLoginPort();
	SafetyNetClient::getInstance()->NewLongSocket(ip, port, "Login");
	SafetyNetClient::getInstance()->Start(nstThread);
	SafetyNetClient::getInstance()->EnterDataFunc(SCT_CALLBACK_2(AppDelegate::DataCallback, this));
	
	this->sendLoginPackage("cclin", "aabbccddee", "aabbccddee");

    return true;
}

void AppDelegate::update(float delta)
{
	this->m_time = this->m_time + delta;
	if (!isExecute&&this->m_time > 0.0f)
	{
		auto engine = LuaEngine::getInstance();
		if (engine->executeScriptFile("main.lua"))
		{
			CCLOG("error execute main.lua");
		}
		isExecute = true;
	}
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}

void AppDelegate::sendLoginPackage(std::string account, std::string pass, std::string inpass)
{
	char szBuffer[_MAX_MSGSIZE] = { 0 };
	//https://blog.csdn.net/xuhuan_wh/article/details/52170503
	CMD_GP_RegisterAccounts* pRegisterAccounts = (CMD_GP_RegisterAccounts*)szBuffer;
	memset(pRegisterAccounts, 0, sizeof(CMD_GP_RegisterAccounts));
	pRegisterAccounts->dwPlazaVersion = MAKELONG(1, 24);

	//_snprintf_s(pRegisterAccounts->szMachineID, LEN_MACHINE_ID, "%S", "cclin");

	strncpy(pRegisterAccounts->szMachineID, "cclin", 5);

	//USES_CONVERSION;

	strncpy(pRegisterAccounts->szLogonPass, pass.c_str(), pass.length());
	strncpy(pRegisterAccounts->szInsurePass, inpass.c_str(), inpass.length());

	pRegisterAccounts->wFaceID = 1;
	pRegisterAccounts->cbGender = rand_0_1() > 0.5 ? 1 : 0;
	strncpy(pRegisterAccounts->szAccounts, account.c_str(), account.length());
	strncpy(pRegisterAccounts->szNickName, "ABCD", 4);
	strncpy(pRegisterAccounts->szSpreader, "CDEF", 4);
	strncpy(pRegisterAccounts->szPassPortID, "350525198205173557", 19);
	strncpy(pRegisterAccounts->szCompellation, "XXX", 3);
	pRegisterAccounts->cbValidateFlags = 1;

	pRegisterAccounts->wDeviceType = 1;//PC¶Ë
	pRegisterAccounts->wRegisterType = 0;//ÓÎ¿Í
	strncpy(pRegisterAccounts->szFaceCode, "https://", 8);

	SafetyLongSocket* socket = (SafetyLongSocket*)SafetyNetClient::getInstance()->getSocket("Login");
	SafetySocket_wanghu* wsocket = (SafetySocket_wanghu*)socket->getSocket();
	WorldPacket* wp = wsocket->newData(socket->getAddress()->nTag, 13, 3, szBuffer, sizeof(CMD_GP_RegisterAccounts));
	SafetyNetClient::getInstance()->SendData((void*)wp->contents(), wp->size(), "Login");

	delete wp;
	//socket->SendData(13, 3, szBuffer, sizeof(CMD_GP_RegisterAccounts));
}

bool AppDelegate::DataCallback(SocketAddress* addr, WorldPacket* wp)
{
	CCLOG("%s %d,%d,%d", addr->nServerIp.c_str(), addr->nPort,wp->m_command,wp->allsize());
	return true;
}