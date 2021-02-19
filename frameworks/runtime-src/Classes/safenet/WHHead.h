 
#ifndef _Msg_Head_
#define _Msg_Head_

#include "cocos2d.h"

#define SOCKET_VER						0x66								//网络版本

#pragma pack(8)

//网络内核
struct CMD_Info
{
	BYTE								cbVersion;							//版本标识
	BYTE								cbCheckCode;						//效验字段
	WORD								wPacketSize;						//数据大小
};

//网络命令
struct CMD_Command
{
	WORD								wMainCmdID;							//主命令码
	WORD								wSubCmdID;							//子命令码
};

//网络包头
struct CMD_Head
{
	CMD_Info							CmdInfo;							//基础结构
	CMD_Command							CommandInfo;						//命令信息
};

//消息头
struct  MsgHead
{
	CMD_Head cmdHead;
	
// 	void Pack(CStreamNet &pStreamNet);
// 	void UnPack(CStreamNet &pStreamNet);

	MsgHead(){}
};

#pragma pack()

#endif
