#pragma once
//#pragma once

#include <string>
using namespace std;

class base64
{
public:
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    */
    string Encode(const unsigned char* Data,int DataByte);
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    OutByte
        [out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ����
        ������ݵĳ���
    */
    int Decode(const unsigned char* Data,unsigned char* oData,int DataByte);
};