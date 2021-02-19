/******************************************************************************** 
 * create:cclin
 * date:2014-12-03
 * ver:
 * latest update:
 * function:Õ¯¬Á ÿª§
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "SafetyShortSocket.h"
#include "cocos2d.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;

enum HttpMethod
{
    hmUnknown
    ,hmGet
    ,hmPost
    ,hmPut
    ,hmDelete
};

class HttpHeader
{
    
};

class HttpBody
{
    
};

class SafetyHttpSocket:public Node
{
public:
    explicit SafetyHttpSocket(string ip,int port,string name);
	~SafetyHttpSocket();

    SafetyShortSocket* getShortSocket(){return m_ShortSocket;};
    
    bool SendHeader(string header);
    bool SendBody(string body);
    
    void EnterDataFunc(sckDataFunc func);
    void ExitDataFunc();
private:
    string m_Tag;
    SafetyShortSocket* m_ShortSocket;
    sckDataFunc m_HttpDataFunc;
    void RenderFunc(float delta);
    
    bool DataCall(SocketAddress* addr,WorldPacket* wp);
    void ConnectCall(SocketAddress* addr,ConnectStatus cs);
    
    WorldPacket* HttpParseContent(WorldPacket* wp);
};