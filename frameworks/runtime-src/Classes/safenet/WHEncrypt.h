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
	BYTE						    m_cbSendRound;					    //�ֽ�ӳ��
	DWORD							m_dwSendPacketCount;				//���ͼ���
	DWORD							m_dwSendXorKey;						//������Կ
	
	BYTE							m_cbRecvRound;						//�ֽ�ӳ��
	DWORD							m_dwRecvPacketCount;				//���ܼ���
	DWORD							m_dwRecvXorKey;						//������Կ
} SocketKey;

class CEncrypt
{
public:	
	CEncrypt();
	~CEncrypt();
	//����
	WORD EncryptBuffer(int tag,BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize);
	//����
	WORD DecryptBuffer(int tag,BYTE pcbDataBuffer[], WORD wDataSize);
	//�յ������ݵ��ֽ�ӳ��
	BYTE MapRecvByte(int tag,BYTE const cbData);
	//���͵����ݵ��ֽ�ӳ��
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