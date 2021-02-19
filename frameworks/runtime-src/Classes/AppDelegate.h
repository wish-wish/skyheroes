#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cocos2d.h"
#include "SafetyNetClient.h"
/**
@brief    The cocos2d Application.

Private inheritance here hides part of interface from Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

	void update(float delta);
	/**
    @brief  Called when the application moves to the background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  Called when the application reenters the foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();
public:
	bool isExecute;
	bool m_type;
	float m_time;

	void sendLoginPackage(std::string account, std::string pass, std::string inpass);
	bool DataCallback(SocketAddress* addr, WorldPacket* wp);
};

#endif  // __APP_DELEGATE_H__

