#ifndef PACKET_AIDE_HEAD_FILE
#define PACKET_AIDE_HEAD_FILE

#pragma once

#include "Macro.h"

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//���ݶ���
#define DTP_NULL					0								//��Ч����

//��������
struct tagDataDescribe
{
	WORD							wDataSize;						//���ݴ�С
	WORD							wDataDescribe;					//��������
};

//////////////////////////////////////////////////////////////////////////////////

//���͸�����
class CSendPacketHelper
{
	//��������
protected:
	WORD							m_wDataSize;					//���ݴ�С
	WORD							m_wMaxBytes;					//�����С
	LPBYTE							m_pcbBuffer;					//����ָ��

	//��������
public:
	//���캯��
	inline CSendPacketHelper(VOID * pcbBuffer, WORD wMaxBytes);

	//���ܺ���
public:
	//��������
	inline VOID CleanData() { m_wDataSize=0; }
	//��ȡ��С
	inline WORD GetDataSize() { return m_wDataSize; }
	//��ȡ����
	inline VOID * GetDataBuffer() { return m_pcbBuffer; }

	//���ܺ���
public:
	//�����ַ�
	inline bool AddPacket(LPCSTR pszString, WORD wDataType);
	//�����ַ�
	inline bool AddPacket(LPCWSTR pszString, WORD wDataType);
	//��������
	inline bool AddPacket(VOID * pData, WORD wDataSize, WORD wDataType);
};

//////////////////////////////////////////////////////////////////////////////////

//���ո�����
class CRecvPacketHelper
{
	//��������
protected:
	WORD							m_wDataPos;						//���ݵ�
	WORD							m_wDataSize;					//���ݴ�С
	LPBYTE							m_pcbBuffer;					//����ָ��

	//��������
public:
	//���캯��
	inline CRecvPacketHelper(LPCVOID pcbBuffer, WORD wDataSize);

	//���ܺ���
public:
	//��ȡ����
	inline VOID * GetData(tagDataDescribe & DataDescribe);
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSendPacketHelper::CSendPacketHelper(VOID * pcbBuffer, WORD wMaxBytes)
{
	//���ñ���
	m_wDataSize=0;
	m_wMaxBytes=wMaxBytes;
	m_pcbBuffer=(BYTE *)pcbBuffer;

	return;
}

//�����ַ�
bool CSendPacketHelper::AddPacket(LPCSTR pszString, WORD wDataType)
{
	//�����ж�
	if (pszString[0]==0) return true;

	//��������
	return AddPacket((VOID *)pszString,CountStringBufferA(pszString),wDataType);
}

//�����ַ�
bool CSendPacketHelper::AddPacket(LPCWSTR pszString, WORD wDataType)
{
	//�����ж�
	if (pszString[0]==0) return true;

	//��������
	return AddPacket((VOID *)pszString,CountStringBufferW(pszString),wDataType);
}

//��������
bool CSendPacketHelper::AddPacket(VOID * pData, WORD wDataSize, WORD wDataType)
{
	//Ч������
	if ((wDataSize+sizeof(tagDataDescribe)+m_wDataSize)>m_wMaxBytes) return false;

	//��������
	tagDataDescribe * pDataDescribe=(tagDataDescribe *)(m_pcbBuffer+m_wDataSize);
	pDataDescribe->wDataSize=wDataSize;
	pDataDescribe->wDataDescribe=wDataType;

	//��������
	if (wDataSize>0)
	{
		CopyMemory(pDataDescribe+1,pData,wDataSize);
	}

	//��������
	m_wDataSize+=sizeof(tagDataDescribe)+wDataSize;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CRecvPacketHelper::CRecvPacketHelper(LPCVOID pcbBuffer, WORD wDataSize)
{
	//���ñ���
	m_wDataPos=0;
	m_wDataSize=wDataSize;
	m_pcbBuffer=(BYTE *)pcbBuffer;

	return;
}

//��ȡ����
VOID * CRecvPacketHelper::GetData(tagDataDescribe & DataDescribe)
{
	//Ч������
	if (m_wDataPos>=m_wDataSize) 
	{
		DataDescribe.wDataSize=0;
		DataDescribe.wDataDescribe=DTP_NULL;
		return NULL;
	}

	//��ȡ����
	CopyMemory(&DataDescribe,m_pcbBuffer+m_wDataPos,sizeof(tagDataDescribe));
	
	//Ч������
	if ((m_wDataPos+sizeof(tagDataDescribe)+DataDescribe.wDataSize)>m_wDataSize)
	{
		DataDescribe.wDataSize=0;
		DataDescribe.wDataDescribe=DTP_NULL;
		return NULL;
	}

	//��������
	VOID * pData=NULL;
	if (DataDescribe.wDataSize>0) pData=m_pcbBuffer+m_wDataPos+sizeof(tagDataDescribe);
	m_wDataPos+=sizeof(tagDataDescribe)+DataDescribe.wDataSize;

	return pData;
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif