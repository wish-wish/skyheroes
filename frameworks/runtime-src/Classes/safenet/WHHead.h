 
#ifndef _Msg_Head_
#define _Msg_Head_

#include "cocos2d.h"

#define SOCKET_VER						0x66								//����汾

#pragma pack(8)

//�����ں�
struct CMD_Info
{
	BYTE								cbVersion;							//�汾��ʶ
	BYTE								cbCheckCode;						//Ч���ֶ�
	WORD								wPacketSize;						//���ݴ�С
};

//��������
struct CMD_Command
{
	WORD								wMainCmdID;							//��������
	WORD								wSubCmdID;							//��������
};

//�����ͷ
struct CMD_Head
{
	CMD_Info							CmdInfo;							//�����ṹ
	CMD_Command							CommandInfo;						//������Ϣ
};

//��Ϣͷ
struct  MsgHead
{
	CMD_Head cmdHead;
	
// 	void Pack(CStreamNet &pStreamNet);
// 	void UnPack(CStreamNet &pStreamNet);

	MsgHead(){}
};

#pragma pack()

#endif
