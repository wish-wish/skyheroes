/******************************************************************************** 
 * create:cclin
 * date:2014-11-5
 * ver:
 * latest update:
 * function:
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#include "platform/CCPlatformConfig.h"
#include "cnTool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hero_xxtea.h"
#ifdef WIN32  
#include "iconv.h"  
#else  
#include <iconv.h>  
#endif 
#include "ForbidWord.h"
#include "gmd5.h"
#include "gbase64.h"

#ifndef ICONV_CONST
# define ICONV_CONST const
#endif


const string EncryKey="hero_key_hero.__hero";
const int CN_SIZE=8;
const int EN_SIZE=16;
const int CN_CHARACTERNAME_SIZE=6;
const int EN_CHARACTERNAME_SIZE=8;
const int PASSWORD_MIN=6;
const int PASSWORD_MAX=16;


bool cnTools::is_zh_ch(char p)
{
// 	if(~(p >> 8) == 0)  //将p字节进行移位运算，右移8位，这样，如果移位后是0，则说明原来的字节最高位为0，不是1那么也就不是汉字的一个字节。
// 	{
// 		return false;  //代表不是汉字
// 	}
// 	return true;  
//	return _ismbslead(p);
//	_ismbstrail
	int ret=0;
	if (p>=0xa1)
		if (p==0xa3)
			ret=1;	//全角字符
		else
			ret=2;	//汉字
	else
		ret=0;	//英文、数字、英文标点
	return ret!=0;
}

int cnTools::IsGB(unsigned short *pText)
{
	unsigned char sqChar[20];
	sqChar[0]=*pText;
	if (sqChar[0]>=0xa1)
		if (sqChar[0]==0xa3)
			return 1;	//全角字符
		else
			return 2;	//汉字
	else
		return 0;	//英文、数字、英文标点
}

bool cnTools::IS_CHINESE_CHAR(unsigned char lo,unsigned char hi)
{
	if(lo<0x81  ||  lo >0xFE )
		return false; 
	if(lo >=0xA1 && lo <= 0xA9)
		return false;//符号和日文
	if(hi <0x40 || hi ==0xFF || hi == 0x7F)
		return false;
	return true;

}

//截取字符，必满乱码的处理
string cnTools::subString(string str ,int start ,int end)
{	
	if(typeid(str)==typeid(string) && str.length()>0)
	{
		int len=str.length();

		string tmp="";
		//先把str里的汉字和英文分开
		vector <string> dump;
		int i=0;
		while(i<len)
		{
			if (is_zh_ch(str.at(i))==1)
			{
				dump.push_back(str.substr(i,3));
				i=i+3;
			}
			else
			{
				dump.push_back(str.substr(i,1));
				i=i+1;
			}
		}
		int iDumpSize = dump.size();
		end=end>0?end:iDumpSize;
		if(start<0||start>end)
			return "";
		for(i=start; i<=end; i++)
		{
			tmp+=dump[i-1];
		}
		return tmp;
	}
	else
	{
		printf("str is not string\n");
		return "";
	}
}

//获取字符串的真实长度处理
unsigned int cnTools::getRealStringLength(string str){
	int i=0;
	int len=str.length();
	unsigned int r=0;
	while(i<len)
	{
		if (is_zh_ch(str.at(i))==1)
		{
			r++;
			i=i+3;
		}
		else
		{
			r++;
			i=i+1;
		}
	}
	return r;
}



string cnTools::getNameBCSRule(string str)
{
	//CALL_SHOW_TIPS(LOCALSTR("length_too_long"));
	string value=str;
	if(str.length()>16)
	{						
		if(cnTools::is_zh_ch(str[16]))
		{
			value=str.substr(0,16);
		}else
		{
			value=str.substr(0,15);
		}						
	}
	return value;
}

bool cnTools::isAccountNameRule(string str)
{		
	if(str.length()<1)
		return false;
	if (!isEnChar(str.at(0))||str.at(0)=='_')
	{ 
		return false;
	}
	for(unsigned int i=1;i<str.length();i++)
	{
		if (!(isNumber(str.at(i)) || isEnChar(str.at(i))||str.at(i)=='_'))
		{ 
			return false;
		}
	}
	return true;
}

bool cnTools::isRoleNameRule(string str)
{
	if(str.length()<1)
		return false;
	if (!(is_zh_ch(str.at(0))||isEnChar(str.at(0))||str.at(0)=='_'))
	{ 
		return false;
	}
	for(unsigned int i=is_zh_ch(str.at(0))?2:1;i<str.length();i++)
	{
		if(is_zh_ch(str.at(i)))
			i++;
		if(i>=str.length()) break;
		if (!(isNumber(str.at(i))||isEnChar(str.at(i))||isSymbol(str.at(i))))
		{ 
			return false;
		}
	}
	return true;
}

bool cnTools::isPassRule(string str)
{
#ifdef WIN32
	if(str.length()>1&&str.length()<=PASSWORD_MAX)
#else
	if(str.length()>=PASSWORD_MIN&&str.length()<=PASSWORD_MAX)
#endif
	{	
		return true;
	}
	return false;
}

string cnTools::getNameUTF8Rule(string str,int cnsize,int ensize)
{
	if(str.length()<2)
		return str;
    
	vector<string> words=ParseUTF8(str);	
	string retstr;
	int count=words.size();
	if(isLongUTF8(str))
		for(int i=0;i<min(cnsize,count);i++)
			retstr+=words[i];
	else
		for(int i=0;i<min(ensize,count);i++)
			retstr+=words[i];
	return retstr;
}

int cnTools::GetStringWidth(string sin)
{
    vector<string> words=ParseUTF8(sin);
    int len=words.size();
    for (int i=0; i<words.size(); i++) {
        if (words[i].length()>1) {
            len+=1;
        }
    }
    return len;
}

vector<string> cnTools::ParseUTF8(string sin) {
	//CCLOG("cnTools::ParseUTF8 %s",sin.c_str());
	int l = sin.length();
	vector<string> ret;
	ret.clear();
	for(int p = 0; p < l; ) {
		int size, n = l - p;
		unsigned char c = sin[p], cc = sin[p + 1];
		if(c < 0x80) {
			size = 1;
		} else if(c < 0xc2) {
			CCLOG("WRONG FROM OF THE SEQUENCE");			
		} else if(c < 0xe0) {
			if(n < 2) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40)) {
				CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 2;
		} else if(c < 0xf0) {
			if(n < 3) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 && 
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(c >= 0xe1 || cc >= 0xa0))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 3;
		} else if(c < 0xf8) {
			if(n < 4) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(c >= 0xf1 || cc >= 0x90))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 4;
		} else if (c < 0xfc) {
			if(n < 5) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x88))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 5;
		} else if (c < 0xfe) {
			if(n < 6) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(sin[p + 5] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x84))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 6;
		} else {
			CCLOG("WRONG FROM OF THE SEQUENCE");
			size=1;
			break;
		}
		string temp = "";
		temp = sin.substr(p, size);
		ret.push_back(temp);
		//CCLOG("Word=%s",temp.c_str());
		p += size;
	}
	return ret;
}

bool cnTools::isLongUTF8(string sin)
{
	int l = sin.length();	
	for(int p = 0; p < l; ) {
		int size, n = l - p;
		unsigned char c = sin[p], cc = sin[p + 1];
		if(c < 0x80) {
			size = 1;
		} else if(c < 0xc2) {
			CCLOG("WRONG FROM OF THE SEQUENCE");			
		} else if(c < 0xe0) {
			if(n < 2) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40)) {
				CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 2;
			return true;
		} else if(c < 0xf0) {
			if(n < 3) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 && 
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(c >= 0xe1 || cc >= 0xa0))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 3;
			return true;
		} else if(c < 0xf8) {
			if(n < 4) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(c >= 0xf1 || cc >= 0x90))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 4;
			return true;
		} else if (c < 0xfc) {
			if(n < 5) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x88))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 5;
			return true;
		} else if (c < 0xfe) {
			if(n < 6) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(sin[p + 5] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x84))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 6;
			return true;
		} else {
			CCLOG("WRONG FROM OF THE SEQUENCE");
		}
		string temp = "";
		temp = sin.substr(p, size);
		p += size;
	}
	return false;
}

int cnTools::sizeOfLongUTF8Char(string sin)
{
	int l = sin.length();	
	for(int p = 0; p < l; ) {
		int size=0, n = l - p;
		unsigned char c = sin[p], cc = sin[p + 1];
		if(c < 0x80) {
			size = 1;
			return size;
		} else if(c < 0xc2) {
			CCLOG("WRONG FROM OF THE SEQUENCE");			
		} else if(c < 0xe0) {
			if(n < 2) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40)) {
				CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 2;
			return size;
		} else if(c < 0xf0) {
			if(n < 3) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 && 
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(c >= 0xe1 || cc >= 0xa0))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 3;
			return size;
		} else if(c < 0xf8) {
			if(n < 4) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 &&
				(c >= 0xf1 || cc >= 0x90))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 4;
			return size;
		} else if (c < 0xfc) {
			if(n < 5) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x88))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 5;
			return size;
		} else if (c < 0xfe) {
			if(n < 6) {
				CCLOG("MISSING FROM THE SEQUENCE");
			}
			if(!((sin[p + 1] ^ 0x80) < 0x40 &&
				(sin[p + 2] ^ 0x80) < 0x40 &&
				(sin[p + 3] ^ 0x80) < 0x40 && 
				(sin[p + 4] ^ 0x80) < 0x40 &&
				(sin[p + 5] ^ 0x80) < 0x40 &&
				(c >= 0xfd || cc >= 0x84))) {
					CCLOG("WRONG FROM OF THE SEQUENCE");
			}
			size = 6;
			return size;
		} else {
			CCLOG("WRONG FROM OF THE SEQUENCE");
		}
		string temp = "";
		temp = sin.substr(p, size);
		p += size;
	}
	return 0;
}

bool cnTools::isNumber(char c)
{
	if ((c>='0' && c<='9'))
	{ 
		return true;
	}
	return false;
}

bool cnTools::isEnChar(char c)
{
	if ((c>='a' && c<='z')||(c>='A' && c<='Z'))
	{ 
		return true;
	}
	return false;
}

bool cnTools::isSymbol(char c)
{
	if (c>='_'||c>='~')
	{ 
		return true;
	}
	return false;
}

bool cnTools::isPassUTF8Rule(string str)
{
#ifdef WIN32
	if(str.length()>1&&str.length()<=PASSWORD_MAX&&!isLongUTF8(str))
#else
	if(str.length()>=PASSWORD_MIN&&str.length()<=PASSWORD_MAX&&!isLongUTF8(str))
#endif
	{	
		return true;
	}
	return false;
}

bool cnTools::isAccountNameUTF8Rule(string str)
{	
	vector<string> words=ParseUTF8(str);	
	string retstr;
	int count=words.size();		
	if(count>0)
	{
		if(!(words[0].length()==1&&(isEnChar(words[0].at(0))||words[0].at(0)=='_')))
		{
			return false;
		}
		if(words[0].length()>1)
			return false;
	}
	for(int i=1;i<count;i++)
	{
		if(words[i].length()==1)
		{
			if(!(isNumber(words[i].at(0))||isEnChar(words[i].at(0))||isSymbol(words[i].at(0))))
			{
				return false;
			}
		}else
		{
			return false;
		}
	}	
	return true;
}

bool cnTools::isRoleNameUTF8Rule(string str)
{
	vector<string> words=ParseUTF8(str);	
	string retstr;
	int count=words.size();		
	if(count>0)
	{
		if(words[0].length()==1&&!(isEnChar(words[0].at(0))||words[0].at(0)=='_'))
		{
			return false;
		}
	}
	for(int i=1;i<count;i++)
	{
		if(words[i].length()==1)
		{
			if(!(isNumber(words[i].at(0))||isEnChar(words[i].at(0))||isSymbol(words[i].at(0))))
			{
				return false;
			}
		}
	}	
	return true;
}

/*!
 对字符串进行语言编码转换
 param from  原始编码，比如"GB2312",的按照iconv支持的写
 param to      转换的目的编码
 param save  转换后的数据保存到这个指针里，需要在外部分配内存
 param savelen 存储转换后数据的内存大小
 param src      原始需要转换的字符串
 param srclen    原始字符串长度
 */
int cnTools::convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen)
{
	//http://www.cnblogs.com/hewei2012/p/3374147.html
	int status = 0;
#ifdef WIN32
    iconv_t cd;
    char   *inbuf = src;
    char *outbuf = save;
    size_t outbufsize = savelen;    
    size_t  savesize = 0;
    size_t inbufsize = srclen;
    const char* inptr = inbuf;
    size_t      insize = inbufsize;
    char* outptr = outbuf;
    size_t outsize = outbufsize;
    
    cd = iconv_open(to, from);
    iconv(cd,NULL,NULL,NULL,NULL);
    if (inbufsize == 0) {
        status = -1;
        goto done;
    }
    while (insize > 0) {
        size_t res = iconv(cd,(ICONV_CONST char**)&inptr,&insize,&outptr,&outsize);
        if (outptr != outbuf) {
            int saved_errno = errno;
            int outsize = outptr - outbuf;
            strncpy(save+savesize, outbuf, outsize);
            errno = saved_errno;
        }
        if (res == (size_t)(-1)) {
            if (errno == EILSEQ) {
                int one = 1;
                iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
                status = -3;
            } else if (errno == EINVAL) {
                if (inbufsize == 0) {
                    status = -4;
                    goto done;
                } else {
                    break;
                }
            } else if (errno == E2BIG) {
                status = -5;
                goto done;
            } else {
                status = -6;
                goto done;
            }
        }
    }
    status = strlen(save);
done:
    iconv_close(cd);
#endif
    return status;
}

bool cnTools::Convert(char* from_charset, char* to_charset, char* inbuf, size_t inlen, char* outbuf, size_t outlen)  
{  
	iconv_t iconvH = iconv_open(to_charset, from_charset);  
	if(!iconvH)  
		return false;  
	memset(outbuf, 0, outlen);  
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32||CC_TARGET_PLATFORM==CC_PLATFORM_WINRT)  
	const char* pTemp = inbuf;  
	const char** pIn = &pTemp;  
	char** pOut = &outbuf;  
	if(!iconv(iconvH, pIn, &inlen, pOut, &outlen))  
	{  
		iconv_close(iconvH);  
		return false;  
	}  
#else  
	if(!iconv(iconvH, &inbuf, &inlen, &outbuf, &outlen))  
	{  
		iconv_close(iconvH);  
		return false;  
	}  
#endif  
	iconv_close(iconvH); 
	return true;
}  

// use std::wstring
/*
wchar_t wchar[] = L"汉字";
NSString *str = [[NSString alloc] initWithBytes:wchar length:sizeof(wchar) encoding:NSUTF32LittleEndianStringEncoding];
*/

/*
It's a problem with the file $NDK/sources/cxx-stl/gnu-libstdc++/Android.mk
Add the following line to that file:
LOCAL_MODULE_FILENAME := libstdc++
*/

/* iconv -l 支持的编码
	437, 500, 500V1, 850, 851, 852, 855, 856, 857, 860, 861, 862, 863, 864, 865, 866, 866NAV, 869, 874, 904, 1026, 1046, 1047, 8859_1, 8859_2, 8859_3, 8859_4, 
	8859_5, 8859_6, 8859_7, 8859_8, 8859_9, 10646-1:1993, 10646-1:1993/UCS4/ANSI_X3.4-1968, ANSI_X3.4-1986, ANSI_X3.4, ANSI_X3.110-1983, ANSI_X3.110, ARABIC, ARABIC7,
	ARMSCII-8, ASCII, ASMO-708, ASMO_449, BALTIC, BIG-5, BIG-FIVE, BIG5-HKSCS, BIG5, BIG5HKSCS, BIGFIVE, BS_4730, CA, CN-BIG5, CN-GB, CN, CP-AR, CP-GR, CP-HU, CP037, 
	CP038, CP273, CP274, CP275, CP278, CP280, CP281, CP282, CP284, CP285, CP290, CP297, CP367, CP420, CP423, CP424, CP437, CP500, CP737, CP775, CP813, CP819, CP850, 
	CP851, CP852, CP855, CP856, CP857, CP860, CP861, CP862, CP863, CP864, CP865, CP866, CP866NAV, CP868, CP869, CP870, CP871, CP874, CP875, CP880, CP891, CP903, CP904, 
	CP905, CP912, CP915, CP916, CP918, CP920, CP922, CP930, CP932, CP933, CP935, CP936, CP937, CP939, CP949, CP950, CP1004, CP1026, CP1046, CP1047, CP1070, CP1079, 
	CP1081, CP1084, CP1089, CP1124, CP1125, CP1129, CP1132, CP1133, CP1160, CP1161, CP1162, CP1163, CP1164, CP1250, CP1251, CP1252, CP1253, CP1254, CP1255, CP1256, 
	CP1257, CP1258, CP1361, CP10007, CPIBM861, CSA7-1, CSA7-2, CSASCII, CSA_T500-1983, CSA_T500, CSA_Z243.4-1985-1, CSA_Z243.4-1985-2, CSA_Z243.419851, 
	CSA_Z243.419852, CSDECMCS, CSEBCDICATDE, CSEBCDICATDEA, CSEBCDICCAFR, CSEBCDICDKNO, CSEBCDICDKNOA, CSEBCDICES, CSEBCDICESA, CSEBCDICESS, CSEBCDICFISE, 
	CSEBCDICFISEA, CSEBCDICFR, CSEBCDICIT, CSEBCDICPT, CSEBCDICUK, CSEBCDICUS, CSEUCKR, CSEUCPKDFMTJAPANESE, CSGB2312, CSHPROMAN8, CSIBM037, CSIBM038, CSIBM273, 
	CSIBM274, CSIBM275, CSIBM277, CSIBM278, CSIBM280, CSIBM281, CSIBM284, CSIBM285, CSIBM290, CSIBM297, CSIBM420, CSIBM423, CSIBM424, CSIBM500, CSIBM851, CSIBM855, 
	CSIBM856, CSIBM857, CSIBM860, CSIBM863, CSIBM864, CSIBM865, CSIBM866, CSIBM868, CSIBM869, CSIBM870, CSIBM871, CSIBM880, CSIBM891, CSIBM903, CSIBM904, CSIBM905, 
	CSIBM918, CSIBM922, CSIBM930, CSIBM932, CSIBM933, CSIBM935, CSIBM937, CSIBM939, CSIBM943, CSIBM1026, CSIBM1124, CSIBM1129, CSIBM1132, CSIBM1133, CSIBM1160, 
	CSIBM1161, CSIBM1163, CSIBM1164, CSIBM11621162, CSISO4UNITEDKINGDOM, CSISO10SWEDISH, CSISO11SWEDISHFORNAMES, CSISO14JISC6220RO, CSISO15ITALIAN, CSISO16PORTUGESE,
	CSISO17SPANISH, CSISO18GREEK7OLD, CSISO19LATINGREEK, CSISO21GERMAN, CSISO25FRENCH, CSISO27LATINGREEK1, CSISO49INIS, CSISO50INIS8, CSISO51INISCYRILLIC, CSISO58GB1988,
	CSISO60DANISHNORWEGIAN, CSISO60NORWEGIAN1, CSISO61NORWEGIAN2, CSISO69FRENCH, CSISO84PORTUGUESE2, CSISO85SPANISH2, CSISO86HUNGARIAN, CSISO88GREEK7, CSISO89ASMO449,
	CSISO90, CSISO92JISC62991984B, CSISO99NAPLPS, CSISO103T618BIT, CSISO111ECMACYRILLIC, CSISO121CANADIAN1, CSISO122CANADIAN2, CSISO139CSN369103, CSISO141JUSIB1002, 
	CSISO143IECP271, CSISO150, CSISO150GREEKCCITT, CSISO151CUBA, CSISO153GOST1976874, CSISO646DANISH, CSISO2022CN, CSISO2022JP, CSISO2022JP2, CSISO2022KR, CSISO2033,
	CSISO5427CYRILLIC, CSISO5427CYRILLIC1981, CSISO5428GREEK, CSISO10367BOX, CSISOLATIN1, CSISOLATIN2, CSISOLATIN3, CSISOLATIN4, CSISOLATIN5, CSISOLATIN6, CSISOLATINARABIC,
	CSISOLATINCYRILLIC, CSISOLATINGREEK, CSISOLATINHEBREW, CSKOI8R, CSKSC5636, CSMACINTOSH, CSNATSDANO, CSNATSSEFI, CSN_369103, CSPC8CODEPAGE437, CSPC775BALTIC, CSPC850MULTILINGUAL,
	CSPC862LATINHEBREW, CSPCP852, CSSHIFTJIS, CSUCS4, CSUNICODE, CSWINDOWS31J, CUBA, CWI-2, CWI, CYRILLIC, DE, DEC-MCS, DEC, DECMCS, DIN_66003, DK, DS2089, DS_2089, E13B/
	EBCDIC-AT-DE-A, EBCDIC-AT-DE, EBCDIC-BE, EBCDIC-BR, EBCDIC-CA-FR, EBCDIC-CP-AR1, EBCDIC-CP-AR2, EBCDIC-CP-BE, EBCDIC-CP-CA, EBCDIC-CP-CH, EBCDIC-CP-DK, EBCDIC-CP-ES,
	EBCDIC-CP-FI, EBCDIC-CP-FR, EBCDIC-CP-GB, EBCDIC-CP-GR, EBCDIC-CP-HE, EBCDIC-CP-IS, EBCDIC-CP-IT, EBCDIC-CP-NL, EBCDIC-CP-NO, EBCDIC-CP-ROECE, EBCDIC-CP-SE, EBCDIC-CP-TR,
	EBCDIC-CP-US, EBCDIC-CP-WT, EBCDIC-CP-YU, EBCDIC-CYRILLIC, EBCDIC-DK-NO-A, EBCDIC-DK-NO, EBCDIC-ES-A, EBCDIC-ES-S, EBCDIC-ES, EBCDIC-FI-SE-A, EBCDIC-FI-SE, EBCDIC-FR, 
	EBCDIC-GREEK, EBCDIC-INT, EBCDIC-INT1, EBCDIC-IS-FRISS, EBCDIC-IT, EBCDIC-JP-E, EBCDIC-JP-KANA, EBCDIC-PT, EBCDIC-UK, EBCDIC-US, EBCDICATDE, EBCDICATDEA, EBCDICCAFR, 
	EBCDICDKNO, EBCDICDKNOA, EBCDICES, EBCDICESA, EBCDICESS, EBCDICFISE, EBCDICFISEA, EBCDICFR, EBCDICISFRISS, EBCDICIT, EBCDICPT, EBCDICUK, EBCDICUS, ECMA-114, ECMA-118,
	ECMA-128, ECMA-CYRILLIC, ECMACYRILLIC, ELOT_928, ES, ES2, EUC-CN, EUC-JISX0213, EUC-JP-MS, EUC-JP, EUC-KR, EUC-TW, EUCCN, EUCJP-MS, EUCJP-OPEN, EUCJP-WIN, EUCJP, EUCKR,
	EUCTW, FI, FR, GB, GB2312, GB13000, GB18030, GBK, GB_1988-80, GB_198880, GEORGIAN-ACADEMY, GEORGIAN-PS, GOST_19768-74, GOST_19768, GOST_1976874, GREEK-CCITT, GREEK, 
	GREEK7-OLD, GREEK7, GREEK7OLD, GREEK8, GREEKCCITT, HEBREW, HP-ROMAN8, HPROMAN8, HU, IBM-856, IBM-922, IBM-930, IBM-932, IBM-933, IBM-935, IBM-937, IBM-939, IBM-943, 
	IBM-1046, IBM-1047, IBM-1124, IBM-1129, IBM-1132, IBM-1133, IBM-1160, IBM-1161, IBM-1162, IBM-1163, IBM-1164, IBM037, IBM038, IBM256, IBM273, IBM274, IBM275, IBM277, 
	IBM278, IBM280, IBM281, IBM284, IBM285, IBM290, IBM297, IBM367, IBM420, IBM423, IBM424, IBM437, IBM500, IBM775, IBM813, IBM819, IBM848, IBM850, IBM851, IBM852, IBM855, 
	IBM856, IBM857, IBM860, IBM861, IBM862, IBM863, IBM864, IBM865, IBM866, IBM866NAV, IBM868, IBM869, IBM870, IBM871, IBM874, IBM875, IBM880, IBM891, IBM903, IBM904, IBM905, 
	IBM912, IBM915, IBM916, IBM918, IBM920, IBM922, IBM930, IBM932, IBM933, IBM935, IBM937, IBM939, IBM943, IBM1004, IBM1026, IBM1046, IBM1047, IBM1089, IBM1124, IBM1129, 
	IBM1132, IBM1133, IBM1160, IBM1161, IBM1162, IBM1163, IBM1164, IEC_P27-1, IEC_P271, INIS-8, INIS-CYRILLIC, INIS, INIS8, INISCYRILLIC, ISIRI-3342, ISIRI3342, ISO-2022-CN-EXT, 
	ISO-2022-CN, ISO-2022-JP-2, ISO-2022-JP-3, ISO-2022-JP, ISO-2022-KR, ISO-8859-1, ISO-8859-2, ISO-8859-3, ISO-8859-4, ISO-8859-5, ISO-8859-6, ISO-8859-7, ISO-8859-8, ISO-8859-9,
	ISO-8859-10, ISO-8859-11, ISO-8859-13, ISO-8859-14, ISO-8859-15, ISO-8859-16, ISO-10646, ISO-10646/UCS2/
	ISO-10646/UCS4/
	ISO-10646/UTF-8/
	ISO-10646/UTF8/
	ISO-CELTIC, ISO-IR-4, ISO-IR-6, ISO-IR-8-1, ISO-IR-9-1, ISO-IR-10, ISO-IR-11, ISO-IR-14, ISO-IR-15, ISO-IR-16, ISO-IR-17, ISO-IR-18, ISO-IR-19, ISO-IR-21, ISO-IR-25, ISO-IR-27, 
	ISO-IR-37, ISO-IR-49, ISO-IR-50, ISO-IR-51, ISO-IR-54, ISO-IR-55, ISO-IR-57, ISO-IR-60, ISO-IR-61, ISO-IR-69, ISO-IR-84, ISO-IR-85, ISO-IR-86, ISO-IR-88, ISO-IR-89, ISO-IR-90, 
	ISO-IR-92, ISO-IR-98, ISO-IR-99, ISO-IR-100, ISO-IR-101, ISO-IR-103, ISO-IR-109, ISO-IR-110, ISO-IR-111, ISO-IR-121, ISO-IR-122, ISO-IR-126, ISO-IR-127, ISO-IR-138, ISO-IR-139,
	ISO-IR-141, ISO-IR-143, ISO-IR-144, ISO-IR-148, ISO-IR-150, ISO-IR-151, ISO-IR-153, ISO-IR-155, ISO-IR-156, ISO-IR-157, ISO-IR-166, ISO-IR-179, ISO-IR-193, ISO-IR-197, ISO-IR-199,
	ISO-IR-203, ISO-IR-209, ISO-IR-226, ISO646-CA, ISO646-CA2, ISO646-CN, ISO646-CU, ISO646-DE, ISO646-DK, ISO646-ES, ISO646-ES2, ISO646-FI, ISO646-FR, ISO646-FR1, ISO646-GB, ISO646-HU,
	ISO646-IT, ISO646-JP-OCR-B, ISO646-JP, ISO646-KR, ISO646-NO, ISO646-NO2, ISO646-PT, ISO646-PT2, ISO646-SE, ISO646-SE2, ISO646-US, ISO646-YU, ISO2022CN, ISO2022CNEXT, ISO2022JP,
	ISO2022JP2, ISO2022KR, ISO6937, ISO8859-1, ISO8859-2, ISO8859-3, ISO8859-4, ISO8859-5, ISO8859-6, ISO8859-7, ISO8859-8, ISO8859-9, ISO8859-10, ISO8859-11, ISO8859-13, ISO8859-14,
	ISO8859-15, ISO8859-16, ISO88591, ISO88592, ISO88593, ISO88594, ISO88595, ISO88596, ISO88597, ISO88598, ISO88599, ISO885910, ISO885911, ISO885913, ISO885914, ISO885915, ISO885916,
	ISO_646.IRV:1991, ISO_2033-1983, ISO_2033, ISO_5427-EXT, ISO_5427, ISO_5427:1981, ISO_5427EXT, ISO_5428, ISO_5428:1980, ISO_6937-2, ISO_6937-2:1983, ISO_6937, ISO_6937:1992,
	ISO_8859-1, ISO_8859-1:1987, ISO_8859-2, ISO_8859-2:1987, ISO_8859-3, ISO_8859-3:1988, ISO_8859-4, ISO_8859-4:1988, ISO_8859-5, ISO_8859-5:1988, ISO_8859-6, ISO_8859-6:1987,
	ISO_8859-7, ISO_8859-7:1987, ISO_8859-7:2003, ISO_8859-8, ISO_8859-8:1988, ISO_8859-9, ISO_8859-9:1989, ISO_8859-10, ISO_8859-10:1992, ISO_8859-14, ISO_8859-14:1998, ISO_8859-15,
	ISO_8859-15:1998, ISO_8859-16, ISO_8859-16:2001, ISO_9036, ISO_10367-BOX, ISO_10367BOX, ISO_69372, IT, JIS_C6220-1969-RO, JIS_C6229-1984-B, JIS_C62201969RO, JIS_C62291984B
	JOHAB, JP-OCR-B, JP, JS, JUS_I.B1.002, KOI-7, KOI-8, KOI8-R, KOI8-T, KOI8-U, KOI8, KOI8R, KOI8U, KSC5636, L1, L2, L3, L4, L5, L6, L7, L8, L10, LATIN-9, LATIN-GREEK-1, LATIN-GREEK,
	LATIN1, LATIN2, LATIN3, LATIN4, LATIN5, LATIN6, LATIN7, LATIN8, LATIN10, LATINGREEK, LATINGREEK1, MAC-CYRILLIC, MAC-IS, MAC-SAMI, MAC-UK, MAC, MACCYRILLIC, MACINTOSH, MACIS, MACUK,
	MACUKRAINIAN, MS-ANSI, MS-ARAB, MS-CYRL, MS-EE, MS-GREEK, MS-HEBR, MS-MAC-CYRILLIC, MS-TURK, MS932, MS936, MSCP949, MSCP1361, MSMACCYRILLIC, MSZ_7795.3, MS_KANJI, NAPLPS, NATS-DANO,
	NATS-SEFI, NATSDANO, NATSSEFI, NC_NC0010, NC_NC00-10, NC_NC00-10:81, NF_Z_62-010, NF_Z_62-010_(1973), NF_Z_62-010_1973, NF_Z_62010, NF_Z_62010_1973, NO, NO2, NS_4551-1, NS_4551-2,
	NS_45511, NS_45512, OS2LATIN1, OSF00010001, OSF00010002, OSF00010003, OSF00010004, OSF00010005, OSF00010006, OSF00010007, OSF00010008, OSF00010009, OSF0001000A, OSF00010020, OSF00010100,
	OSF00010101, OSF00010102, OSF00010104, OSF00010105, OSF00010106, OSF00030010, OSF0004000A, OSF0005000A, OSF05010001, OSF100201A4, OSF100201A8, OSF100201B5, OSF100201F4, OSF100203B5,
	OSF1002011C, OSF1002011D, OSF1002035D, OSF1002035E, OSF1002035F, OSF1002036B, OSF1002037B, OSF10010001, OSF10020025, OSF10020111, OSF10020115, OSF10020116, OSF10020118, OSF10020122,
	OSF10020129, OSF10020352, OSF10020354, OSF10020357, OSF10020359, OSF10020360, OSF10020364, OSF10020365, OSF10020366, OSF10020367, OSF10020370, OSF10020387, OSF10020388, OSF10020396,
	OSF10020402, OSF10020417, PT, PT2, PT154, R8, RK1048, ROMAN8, RUSCII, SE, SE2, SEN_850200_B, SEN_850200_C, SHIFT-JIS, SHIFT_JIS, SHIFT_JISX0213, SJIS-OPEN, SJIS-WIN, SJIS, SS636127,
	STRK1048-2002, ST_SEV_358-88, T.61-8BIT, T.61, T.618BIT, TCVN-5712, TCVN, TCVN5712-1, TCVN5712-1:1993, TIS-620, TIS620-0, TIS620.2529-1, TIS620.2533-0, TIS620, TS-5881, TSCII, UCS-2,
	UCS-2BE, UCS-2LE, UCS-4, UCS-4BE, UCS-4LE, UCS2, UCS4, UHC, UJIS, UK, UNICODE, UNICODEBIG, UNICODELITTLE, US-ASCII, US, UTF-7, UTF-8, UTF-16, UTF-16BE, UTF-16LE, UTF-32, UTF-32BE,
	UTF-32LE, UTF7, UTF8, UTF16, UTF16BE, UTF16LE, UTF32, UTF32BE, UTF32LE, VISCII, WCHAR_T, WIN-SAMI-2, WINBALTRIM, WINDOWS-31J, WINDOWS-874, WINDOWS-936, WINDOWS-1250, WINDOWS-1251,
	WINDOWS-1252, WINDOWS-1253, WINDOWS-1254, WINDOWS-1255, WINDOWS-1256, WINDOWS-1257, WINDOWS-1258, WINSAMI2, WS2, YU,
*/

bool cnTools::isForbidWord(string sin)
{
	return isInForbidWord(sin);
}

void cnTools::GBKToUTF8(char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
	 Convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

string cnTools::GbKToUTF8String(string in)
{
	int len=in.length()*3+2;
	char* c=(char*)malloc(len);
	memset(c,0,len);
	GBKToUTF8((char*)in.c_str(),in.length(),c,len);
	string s(c);
	free(c);
	return s;
}

string cnTools::Encrypt(string in)
{
	return in;
	//TODO:Android 会崩溃+不稳定，会崩溃
	CCLOG("cnTools::Encrypt %s",in.c_str());
//#ifdef ANDROID
	xxtea_long retlen=0;
	unsigned char* c=hero_xxtea_encrypt((unsigned char*)in.c_str(),in.length(),(unsigned char*)EncryKey.c_str(),EncryKey.length(),&retlen);
	string s((char*)c);
	if(c!=NULL)
		free(c);
	if(s.compare("")==0)
		s = in;
	return s;
// #else
// 	return in;
// #endif
}

string cnTools::Decrypt(string in)
{
	return in;
	//TODO:Android 会崩溃+不稳定，会崩溃
	CCLOG("cnTools::Decrypt %s",in.c_str());
//#ifdef ANDROID
	xxtea_long retlen=0;
	unsigned char* c=hero_xxtea_decrypt((unsigned char*)in.c_str(),in.length(),(unsigned char*)EncryKey.c_str(),EncryKey.length(),&retlen);
	string s((char*)c);
	if(c!=0)
		free(c);
	if(s.compare("")==0)
		s=in;
	return s;
// #else
// 	return in;
// #endif
}

string cnTools::MD5(string in)
{
	CCLOG("cnTools::MD5 %s",in.c_str());
	char buf[64] = {0};
	memset(buf,0,64);
	sprintf(buf,"%s",in.c_str());
	Crypto::MD5 md5(buf,strlen(buf));
	std::string newsign = md5.tostring();
	return newsign;
}

string cnTools::EncodeBase64(string in)
{
	base64 b64;
	return b64.Encode((unsigned char*)in.c_str(),in.length());	
}

string cnTools::DecodeBase64(string in)
{
	base64 b64;
	unsigned char unbase[200000];
    memset(unbase, 0, 200000);
	int length=b64.Decode((unsigned char*)in.c_str(),unbase,in.length());
    return string((char*)unbase);
}

bool cnTools::isIPAddr(string ip)
{
	const char* del=".";
	char* token=strtok((char*)ip.c_str(),del);
	int i=0,tem=0;
	bool flag=true;
	while(token!=0)
	{
		i++;
		if(!isAllNumber(token))
		{
			CCLOG("ip token not all number token=%s",token);
			flag=false;
			return false;
		}
		tem=atoi(token);		
		if(tem>255||tem<0)
		{
			CCLOG("ip num>0&&num<255 num=%d",tem);
			flag=false;
			return false;
		}
		token=strtok(0,del);
		if(i>=5) 
		{
			CCLOG("ip section len error len %d",i);
			flag=false;
			return false;
		}
	}
	if(i<4&&flag) 
	{
		CCLOG("ip section len error %d",i);
		flag=false;
		return false;
	}	
	return flag;
}

bool cnTools::isAllNumber(string nums)
{
	for(int i=0;i<nums.size();i++)
	{
		if(!isNumber(nums.at(i)))
			return false;
	}
	return true;
}

bool cnTools::isIPPort(int ip)
{
	if(ip>0&&ip<=65535)
		return true;
	return false;
}

