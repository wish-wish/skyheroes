/******************************************************************************** 
 * create:cclin
 * date:2014-11-5
 * ver:
 * latest update:
 * function:
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

extern const int CN_CHARACTERNAME_SIZE;
extern const int EN_CHARACTERNAME_SIZE;
extern const int CN_SIZE;
extern const int EN_SIZE;
extern const int PASSWORD_MIN;
extern const int PASSWORD_MAX;
extern const string EncryKey;

class cnTools
{
public:
	static bool is_zh_ch(char p);
	static string subString(string str ,int start ,int end);
	static unsigned int getRealStringLength(string str);
	static string getNameBCSRule(string str);	
	static bool isAccountNameRule(string str);
	static bool isRoleNameRule(string str);
	static bool isPassRule(string str);
	static int IsGB(unsigned short *pText);
	static bool IS_CHINESE_CHAR(unsigned char lo,unsigned char hi);

	static bool isNumber(char c);
	static bool isEnChar(char c);
	static bool isSymbol(char c);

	static bool isAccountNameUTF8Rule(string str);
	static bool isRoleNameUTF8Rule(string str);
	static bool isPassUTF8Rule(string str);
	static string getNameUTF8Rule(string str,int cnsize=CN_SIZE,int ensize=EN_SIZE);		

	static vector<string> ParseUTF8(string sin);
    static int GetStringWidth(string sin);//on world equal two
	static bool isLongUTF8(string sin);
	static int  sizeOfLongUTF8Char(string sin);	
	
	static bool isForbidWord(string sin);
	static string GbKToUTF8String(string in);
	static string Encrypt(string in);
	static string Decrypt(string in);

	static string MD5(string in);
	static string EncodeBase64(string in);
	static string DecodeBase64(string in);

	static bool isIPAddr(string ip);
	static bool isAllNumber(string nums);
	static bool isIPPort(int ip);

	static void GBKToUTF8(char* inbuf, size_t inlen, char* outbuf, size_t outlen);
	static bool Convert(char* from_charset, char* to_charset, char* inbuf, size_t inlen, char* outbuf, size_t outlen);	
	static int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen);
};
