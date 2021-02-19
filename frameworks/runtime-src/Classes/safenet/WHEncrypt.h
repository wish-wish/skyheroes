#ifndef __NetEncrypt_H__
#define __NetEncrypt_H__

#include "Singleton.h"
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

typedef struct SKey
{
	BYTE						    m_cbSendRound;					    //字节映射
	DWORD							m_dwSendPacketCount;				//发送计数
	DWORD							m_dwSendXorKey;						//发送密钥
	
	BYTE							m_cbRecvRound;						//字节映射
	DWORD							m_dwRecvPacketCount;				//接受计数
	DWORD							m_dwRecvXorKey;						//接收密钥
} SocketKey;

class CEncrypt
{
public:	
	CEncrypt();
	~CEncrypt();
	//加密
	WORD EncryptBuffer(int tag,BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize);
	//解密
	WORD DecryptBuffer(int tag,BYTE pcbDataBuffer[], WORD wDataSize);
	//收到的数据的字节映射
	BYTE MapRecvByte(int tag,BYTE const cbData);
	//发送的数据的字节映射
	BYTE MapSendByte(int tag,BYTE const cbData);
	WORD SeedRandMap(int tag,WORD wSeed);

	void ResetFlags(int tag);

	SocketKey* isKeyExists(int tag);
	SocketKey* addOrGetKey(int tag);

	void ClearKeys();
	void RemoveKey(int tag);
private:
	map<int,SocketKey*> mKeyMap;
	void ResetFlags();
};

#endif