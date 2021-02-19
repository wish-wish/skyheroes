#include "ForbidWord.h"

vector<string> result;

vector<string> getForbidWord()
{
	if(!result.empty())
		return result;
	string forbidword=FileUtils::getInstance()->getStringFromFile("config/__fbw_.bin");
	string::size_type pos;	
	string pattern=",";
	string words=forbidword+pattern;//扩展字符串以方便操作
	int size=words.size();
	result.clear();
	for(int i=0; i<size; i++)
	{
		pos=words.find(pattern,i);
		if(pos<size)
		{
			std::string s=words.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

bool isInForbidWord(string word)
{
	if(result.empty())
		getForbidWord();
	for(int i=0;i<result.size();i++)
	{
		if(result[i].compare(word)==0)
		{
			return true;
		}
	}
	return false;
}
