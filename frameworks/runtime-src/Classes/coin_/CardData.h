#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "json/rapidjson.h"
#include "json/document.h"

USING_NS_CC;
using namespace std;
//using namespace rapidjson;

#define maxRCount 15
#define defRow 5

typedef vector<int>	   NumberList;
typedef vector<int>    IndexList;


struct MergePos
{
    int x;
    int y;
};

typedef vector<MergePos*> TPosList;

struct MergeObj
{
    int dstx;
    int dsty;
    TPosList PosList;
};

typedef vector<MergeObj*> MergeList;

class CardDispData
{
public:
    explicit CardDispData(int Rows);
    ~CardDispData();
    void setData(int x,int y,int64_t value);
	int64_t getData(int x, int y);
    
	void setTerrainData(int x, int y, int value);
	int getTerrainData(int x, int y);

	void setAliasi(int x, int y, int value);
	int getAliasi(int x, int y);
    
    CardDispData* getPriorData();
    void setPriorData(CardDispData* data);
    
    CardDispData* getNextData();
    void setNextData(CardDispData* data);
    
    NumberList* getNumberList();
	IndexList* getIndexList();
    
    int GetNewPos();
    void SetNewPos(int value);
    int GetDirection();
    void SetDirection(int value);
    
    string toJson();
	rapidjson::Document& toJsonValue(rapidjson::Document& doc, rapidjson::Document::AllocatorType& allocator);
    void fromJson(string json);
	void formJsonValue(rapidjson::Value& array, rapidjson::Value& array2);
	std::vector<std::string> splitStr(std::string astr, std::string asplit);
	void fromString(std::string str, std::string inds = "");
    bool isLeaf(){return m_NextData==nullptr;};
    
    MergeList* getMergeList();
    void clearMergeList(MergeList* list);
    
    bool isInMerge(MergeList* list,int x,int y);
    bool isInMergeFirst(MergeList* list,int x,int y);
	bool isSameMerge(MergeList* list, int x, int y, TPosList* plist);
    bool isSameMerge(MergeList* list,int x,int y, int x1,int y1);
    MergeObj* getMergeObj(MergeList* list,int x,int y);
    void setSceneName(string name){m_SceneName=name;};
    string getSceneName(){return m_SceneName;};

	int getRow(){ return m_Rows; }
	
private:
    int m_Rows;
    int m_NewPos;
    int m_Direction;
    int m_CoundSeconds;
    string m_SceneName;
    
    NumberList m_NumberList;
	IndexList  m_AliasIndexList;
    MergeList m_MergeList;
    
    CardDispData* m_PriorData;
    CardDispData* m_NextData;
    
    char TerrainCode[maxRCount][maxRCount];
};

struct TransNumber
{
    int64_t Intnum;
	int64_t Max;
    string transName;
    string picture;
    string color;
	int64_t goal;
	int type;
    bool isTerrain;
	std::vector<std::string> aliasNames;
    TransNumber()
    {
        isTerrain=false;
    }
};

typedef vector<CardDispData*> StepList;

typedef vector<TransNumber*> NameList;

struct NameListObj
{
    string m_name;
    int64_t m_max;
    string m_unit;
	int m_divide;
    NameList m_List;
    NameList m_AntiList;
};

typedef map<string,NameListObj*> MapNameList;

struct MaxScore
{
    int m_Rows;
    string m_SceneName;
    
    int m_MaxScore;
    int m_MaxValue;
};

typedef vector<MaxScore*> MaxScoreList;

struct TerrainElem
{
    Vec2 Pos;
    int code;
    void *data;
};

typedef vector<TerrainElem*> TerrainData;

struct CardGameData
{
    int64_t m_Score;
    int64_t m_CountSeconds;
    bool  m_IsDefRowDirty;
    MaxScore m_MaxScore;
    
    StepList m_StepList;
    
    //TerrainData m_Terrain;
	NumberList m_EmptyList;
    
	NumberList m_BorderEmptyList;//bian
	NumberList m_CornerEmptyList;//jiao
	NumberList m_InnerEmptyList;//limian
	NumberList m_BorderOneEmptyList;//bianyi
	NumberList m_InnerOneEmptyList;//limianyi
	NumberList m_DuiJiaoEmptyList;//duijiao
    
    CardDispData* m_PlayData;// root linker for the steplist
};

class CardDispManager:public Singleton<CardDispManager>
{
public:
    CardDispManager();
    ~CardDispManager();
    
    CardDispData* AddOneStep();
    
    CardDispData* GetRootStep();
    CardDispData* GetRootStep(CardDispData* data);
    CardDispData* GetLeafStep();
    CardDispData* GetLeafStep(CardDispData* data);
    
    void RePlaySteps();
    
    void writeDataList();
    CardDispData* readDataList();

    void writeConfig();
    void readConfig();
    
    void clearData();
    void removeStep(CardDispData* data);
    CardDispData* resetLeafSetp(CardDispData* data);
    
    NameList* getTransList(string type);
    NameList* getAntiTransList(string type);
    
	void loadTransJson(string file);
    
	string getTransName(string name, int64_t num,int *aliasi);
    bool isTransValueExists(string name,int64_t num);
    int64_t getTransValue(string name,string val);
    string getTransPicture(string name,string val);
    string getTransPicture(string name,int64_t val);
    string getTransUnit(string name);
    string getGoalStr(string name,int64_t num);
	bool isInList(vector<string> &list, string name);
    
    TransNumber* getNextNumber(string name,int64_t num);
    TransNumber* getTransNumber(string name,int64_t num);
    
    void setMaxNumber(int64_t num);
    int64_t getMaxNumber();
    
    void setMaxScore(int64_t num);
    int64_t getMaxScore();
    
    void SetDefRows(int Rows);
    int  GetDefRows();
    
    void setCurrentScene(string name);
    string getCurrentScene();
    
    void setNewScene(string name);
    string getNewScene();
    
    void setIsSound(bool is);
    bool getIsSound();
    
    void setPriorType(int is);
    int getPriorType();
    
    void setIsReturn(bool is){m_IsBack=is;};
    bool getIsReturn(){return m_IsBack;};
    
    void freeTerrain();
    void generateTerrain();
    
    CardGameData* getGameData();
    void getEmptyList();
    
private:
    string  m_CurrentScene;
    string  m_NewScene;
    int m_DefRows;
    int64_t m_MaxNumber;
    int64_t m_MaxScore;
    bool m_IsSound;
    bool m_IsBack;
    int m_PriorType;
    
    CardGameData m_GameData;//game data
    
    MapNameList m_TransMap;
    MaxScoreList m_ScoreList;

    void removeStepFromList(CardDispData* data);
    void freeNameLists();
    void freeScoreList();
    
};

int CardRandom();




