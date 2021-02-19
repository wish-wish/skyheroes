#include "CardData.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "json/reader.h"

using namespace std;
USING_NS_CC;

const string dataLog = "data_log.dat";
const string dataConfig = "data_config.dat";
const string CoinNumberFile = "res/coin/coins.json";

int CardRandom()
{
    long long  tl=time(0);
    std::srand((unsigned int)tl);
    int  ranint=rand();
    return ranint;
}

CardDispData::CardDispData(int Rows)
{
    m_Rows=Rows;
    m_NextData = nullptr;
    m_PriorData = nullptr;
	m_NewPos = 0;
	m_Direction = 0;
    for (int i=0; i<m_Rows; i++) {
        for (int j=0; j<m_Rows; j++) {
            m_NumberList.push_back(0);
			m_NumberList.push_back(0);
			m_AliasIndexList.push_back(-1);
            //TerrainCode[i][j]=0;
        }
    }
}

CardDispData::~CardDispData()
{
	m_NextData = nullptr;
	m_PriorData = nullptr;
}

rapidjson::Document& CardDispData::toJsonValue(rapidjson::Document& doc, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value v(rapidjson::kObjectType);
    v.AddMember("index",0,allocator);
    v.AddMember("Rows", m_Rows, allocator);
    v.AddMember("NewPos",m_NewPos,allocator);
    v.AddMember("Direction",m_Direction,allocator);
    v.AddMember("SceneName", rapidjson::Value(m_SceneName.c_str(), allocator ),allocator);
    doc.AddMember("step",v,allocator);
    
	rapidjson::Value jcard(rapidjson::kArrayType);
    for (int i=0; i<m_NumberList.size(); i++) {
        jcard.PushBack(m_NumberList[i], allocator);
    }
    doc.AddMember("cards",jcard,allocator);
	
	rapidjson::Value jindexs(rapidjson::kArrayType);
	for (int i = 0; i < m_AliasIndexList.size(); i++) {
		jindexs.PushBack(m_AliasIndexList[i], allocator);
	}
	doc.AddMember("indexs", jindexs, allocator);
	rapidjson::Value jmoves(rapidjson::kArrayType);
    doc.AddMember("moves", jmoves, allocator);
    doc.AddMember("merge", "[]", allocator);
    return doc;
}

string CardDispData::toJson()
{
	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();
    
    this->toJsonValue(doc,allocator);
    
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    string json_str = buffer.GetString();
    return json_str;
}

void CardDispData::fromJson(string json)
{
	rapidjson::Document doc;
    doc.Parse<0>(json.c_str());
    rapidjson::Value &val=doc["step"];
    m_SceneName = val["SceneName"].GetString();
    m_NewPos = val["NewPos"].GetInt();
    m_Direction = val["Direction"].GetInt();
    m_Rows = val["Rows"].GetInt();
    rapidjson::Value &array=doc["cards"];
	rapidjson::Value &array2 = doc["indexs"];
	this->formJsonValue(array, array2);
}

void CardDispData::formJsonValue(rapidjson::Value& array, rapidjson::Value& array2)
{
	int allnum = this->getRow()*this->getRow();
	for (unsigned int i = 0; i < allnum*2; i++)
	{
		if (m_NumberList.size() < allnum * 2) {
			m_NumberList.push_back(0);
		}
		double val = array[i].GetDouble();
		m_NumberList[i] = val;
	}
	for (unsigned int i = 0; i<allnum; i++)
    {
		if(m_AliasIndexList.size() < allnum)
		{
			m_AliasIndexList.push_back(-1);
		}
		//fix for old version
		double val = array[i].GetDouble();
		m_NumberList[i] = val;
		if (abs(val - floor(val)) > 0.4)
		{
			if (m_NumberList[i + allnum] == 0)
			{
				m_NumberList[i + allnum] = val + 0.5;
			}
			m_NumberList[i] = 0;
		}
		if (m_AliasIndexList.size() < allnum)
		{
			m_AliasIndexList.push_back(-1);
		}
		if(m_AliasIndexList.size() > i)
		{
			m_AliasIndexList[i] = array[i].GetInt();
		}
    }
}

std::vector<std::string> CardDispData::splitStr(std::string astr, std::string asplit)
{
	std::vector<std::string> ressult;
	char* p = strtok((char*)astr.c_str(), asplit.c_str());
	while (p != NULL)
	{
		ressult.push_back(std::string(p));
		p = strtok(NULL, asplit.c_str());
	}
	return ressult;
}


void CardDispData::fromString(std::string str,std::string inds)
{
	std::vector<string> listnum = this->splitStr(str, ",");
	std::vector<string> listinds = this->splitStr(inds, ",");
	int allnum = this->getRow()*this->getRow();
	for (unsigned int i = 0; i < allnum * 2; i++)
	{
		if (m_NumberList.size() < allnum * 2) {
			m_NumberList.push_back(0);
		}
		double val = atof(listnum[i].c_str());
		m_NumberList[i] = val;
	}
	for (unsigned int i = 0; i < allnum; i++)
	{
		if (m_AliasIndexList.size() < allnum)
		{
			m_AliasIndexList.push_back(-1);
		}
		double val = atof(listnum[i].c_str());
		m_NumberList[i] = val;
		if (abs(val - floor(val)) > 0.4)
		{
			if (m_NumberList[i + allnum] == 0)
			{
				m_NumberList[i + allnum] = val + 0.5;
			}
			m_NumberList[i] = 0;
		}
		if (listinds.size() >i)
		{
			m_AliasIndexList[i] = atoi(listinds[i].c_str());
		}
	}
}

int CardDispData::GetNewPos()
{
    return m_NewPos;
}

void CardDispData::SetNewPos(int value)
{
    m_NewPos = value;
}

int CardDispData::GetDirection()
{
    return m_Direction;
}

void CardDispData::SetDirection(int value)
{
    m_Direction = value;
}

void CardDispData::setData(int x,int y,int64_t value)
{
    int index=x*m_Rows+y;
    m_NumberList[index]=value;
}


int64_t CardDispData::getData(int x, int y)
{
	int index = x*m_Rows + y;
	return m_NumberList[index];
}

void CardDispData::setTerrainData(int x, int y, int value)
{
	int index = x*m_Rows + y + m_Rows*m_Rows;
    m_NumberList[index]=value;
}


int CardDispData::getTerrainData(int x, int y)
{
	int index = x*m_Rows + y+ m_Rows*m_Rows;
	if (m_NumberList.size() > index)
	{
		return m_NumberList[index];
	}
	else
	{
		return 0;
	}
}


void CardDispData::setAliasi(int x, int y, int value)
{
	int index = x*m_Rows + y;
	m_AliasIndexList[index]=value;
}


int CardDispData::getAliasi(int x, int y)
{
	int index = x*m_Rows + y;
	return m_AliasIndexList[index];
}

CardDispData* CardDispData::getPriorData()
{
    return m_PriorData;
}

void CardDispData::setPriorData(CardDispData* data)
{
    m_PriorData = data;
}

CardDispData* CardDispData::getNextData()
{
    return m_NextData;
}

void CardDispData::setNextData(CardDispData* data)
{
    m_NextData = data;
}

NumberList* CardDispData::getNumberList()
{
    return &m_NumberList;
}


IndexList* CardDispData::getIndexList()
{
	return &m_AliasIndexList;
}

void CardDispData::clearMergeList(MergeList* list)
{
    for (int i=0; i<(*list).size(); i++) {
        MergeObj* mergeobj=(*list)[i];
        for(int j=0; j<mergeobj->PosList.size();j++)
        {
            delete mergeobj->PosList[j];
        }
        mergeobj->PosList.clear();
        delete mergeobj;
    }
    (*list).clear();
}

bool CardDispData::isInMerge(MergeList* list,int x,int y)
{
    for (int i=0; i<(*list).size(); i++) {
        MergeObj* mergeobj=(*list)[i];
        for(int j=0; j<mergeobj->PosList.size();j++)
        {
            if (mergeobj->PosList[j]->x==x&&mergeobj->PosList[j]->y==y) {
                return true;
            }
        }
    }
    return false;
}

bool CardDispData::isSameMerge(MergeList* list,int x,int y, int x1,int y1)
{
    MergeObj* ob=this->getMergeObj(list,x,y);
    MergeObj* ob1=this->getMergeObj(list,x1,y1);
    if(ob==ob1&&this->isInMerge(list,x,y))
    {
        return true;
    }
    return false;
}

MergeObj* CardDispData::getMergeObj(MergeList* list,int x,int y)
{
    for (int i=0; i<(*list).size(); i++) {
        MergeObj* mergeobj=(*list)[i];
        for(int j=0; j<mergeobj->PosList.size();j++)
        {
            if (mergeobj->PosList[j]->x==x&&mergeobj->PosList[j]->y==y) {
                return mergeobj;
            }
        }
    }
    return nullptr;
}


bool CardDispData::isInMergeFirst(MergeList* list,int x,int y)
{
    for (int i=0; i<(*list).size(); i++) {
        MergeObj* mergeobj=(*list)[i];
        if (mergeobj->PosList[0]->x==x&&mergeobj->PosList[0]->y==y) {
            return true;
        }
    }
    return false;
}


bool CardDispData::isSameMerge(MergeList* list, int x, int y, TPosList* plist)
{
	for (int i = 0; i < plist->size(); i++)
	{
		if (this->isSameMerge(list, x, y, (*plist)[i]->x, (*plist)[i]->y))
		{
			return true;
		}
	}
	return false;
}

MergeList* CardDispData::getMergeList()
{
    return  &m_MergeList;
}


CardDispManager::CardDispManager()
{
    m_IsBack = false;
    m_GameData.m_Score = 0;
    m_GameData.m_MaxScore.m_MaxValue = 0;
    m_GameData.m_MaxScore.m_MaxScore = 0;
    m_GameData.m_PlayData=nullptr;
    m_GameData.m_CountSeconds = 0;
    this->loadTransJson(CoinNumberFile);
    this->readConfig();
    this->generateTerrain();
}

CardDispManager::~CardDispManager()
{
    this->writeDataList();
    this->writeConfig();
    this->freeNameLists();
    this->freeScoreList();
    this->freeTerrain();
}

void CardDispManager::getEmptyList()
{
    m_GameData.m_EmptyList.clear();
    m_GameData.m_CornerEmptyList.clear();
    m_GameData.m_BorderEmptyList.clear();
    m_GameData.m_InnerEmptyList.clear();
    m_GameData.m_BorderOneEmptyList.clear();
    m_GameData.m_InnerOneEmptyList.clear();
    m_GameData.m_DuiJiaoEmptyList.clear();
	int row = this->GetLeafStep()->getRow();
    for (int i=0; i<row*row; i++) {
		if (this->GetLeafStep()->getData(i / row, i%row) == 0 && this->GetLeafStep()->getTerrainData(i / row, i%row)==0) {
            m_GameData.m_EmptyList.push_back(i);
			int r = i / row;
			int c = i % row;
            
            if ((r==0||r==GetDefRows()-1)&&(c==0||c==GetDefRows()-1))
            {
                m_GameData.m_CornerEmptyList.push_back(i);
            }
            else if (r==0||c==0||r==GetDefRows()-1||c==GetDefRows()-1)
            {
                m_GameData.m_BorderEmptyList.push_back(i);
            }
            else
            {
                m_GameData.m_InnerEmptyList.push_back(i);
                if ((r==1||c==1||r==GetDefRows()-2||c==GetDefRows()-2)&&(GetDefRows()-2>=1)) {
                    m_GameData.m_BorderOneEmptyList.push_back(i);
                }
                else
                {
                    m_GameData.m_InnerOneEmptyList.push_back(i);
                }
            }
            if (((r+c)==GetDefRows()-1)||(r==c)) {
				m_GameData.m_DuiJiaoEmptyList.push_back(i);
            }
        }
    }
}

    
CardGameData* CardDispManager::getGameData()
{
    return &m_GameData;
}

void CardDispManager::writeDataList()
{
    //todo:write data
    CardDispData* step=m_GameData.m_PlayData;
    
	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();
	rapidjson::Value data(rapidjson::kArrayType);
    
    while (step!=nullptr) {
        string str=step->toJson();
        data.PushBack(rapidjson::Value(str.c_str(), allocator ), allocator);
        step=step->getNextData();
    }
    
    doc.AddMember("Rows", m_DefRows, allocator);
    doc.AddMember("Logs", data, allocator);
    
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    string json_str = buffer.GetString();
    
    const string dfile = FileUtils::getInstance()->getWritablePath()+dataLog;
    FILE *fp = fopen(dfile.c_str(), "wb");
    fwrite(json_str.c_str(), 1, json_str.length(), fp);
    fclose(fp);
}

CardDispData* CardDispManager::readDataList()
{
    const string dfile = FileUtils::getInstance()->getWritablePath()+dataLog;
    string json_str=FileUtils::getInstance()->getStringFromFile(dfile);
    
    this->clearData();
    
	rapidjson::Document doc;
    doc.Parse<0>(json_str.c_str());
    
    rapidjson::Value& array=doc["Logs"];
    for(int i=0;i<array.Size();i++)
    {
        CardDispData *data=this->AddOneStep();
		data->formJsonValue(array[i]["cards"], array[i]["indexs"]);
    }
    
    return m_GameData.m_PlayData;
}



void CardDispManager::writeConfig()
{
    //TODO:scene & row config attribute
    //TODO:
	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("SceneName",rapidjson::Value(m_CurrentScene.c_str(),allocator), allocator);
    doc.AddMember("DefRows", m_DefRows, allocator);
    doc.AddMember("IsSound", m_IsSound, allocator);
    doc.AddMember("PriorType", m_PriorType, allocator);
    
	rapidjson::Value data(rapidjson::kArrayType);
    for (int i=0; i<m_ScoreList.size(); i++) {
		rapidjson::Value score(rapidjson::kObjectType);
        score.AddMember("SceneName", rapidjson::Value(m_ScoreList[i]->m_SceneName.c_str(),allocator), allocator);
        score.AddMember("Rows", m_ScoreList[i]->m_Rows, allocator);
        score.AddMember("MaxScore", m_ScoreList[i]->m_MaxScore, allocator);
        score.AddMember("MaxValue", m_ScoreList[i]->m_MaxValue, allocator);
        data.PushBack(score, allocator);
    }
    doc.AddMember("Scores", data, allocator);
    
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    string json_str = buffer.GetString();
    
    const string dfile = FileUtils::getInstance()->getWritablePath()+dataConfig;
    FileUtils::getInstance()->writeStringToFile(json_str.c_str(), dfile);
}

void CardDispManager::readConfig()
{
    const string dfile = FileUtils::getInstance()->getWritablePath()+dataConfig;
    if (FileUtils::getInstance()->isFileExist(dfile)) {
        string json_str=FileUtils::getInstance()->getStringFromFile(dfile);
		rapidjson::Document doc;
        doc.Parse<0>(json_str.c_str());
        
        m_CurrentScene = doc["SceneName"].GetString();
        m_DefRows      = doc["DefRows"].GetInt();
        m_IsSound      = doc["IsSound"].GetBool();
        if(!doc["PriorType"].IsNull())
        {
            m_PriorType    = doc["PriorType"].GetInt();
        }
        else
        {
            m_PriorType    = 0;
        }
        
        rapidjson::Value& scores=doc["Scores"];
        for (int i=0; i<scores.Size(); i++) {
            MaxScore* _score=new MaxScore();
            _score->m_SceneName=scores[i]["SceneName"].GetString();
            _score->m_Rows=scores[i]["Rows"].GetInt();
            _score->m_MaxScore=scores[i]["MaxScore"].GetInt();
            _score->m_MaxValue=scores[i]["MaxValue"].GetInt();
            m_ScoreList.push_back(_score);
        }
    }else
    {
        m_CurrentScene="SceneMoney";
        m_DefRows=defRow;
        m_IsSound=true;
        m_PriorType = 0;
    }
}

void CardDispManager::clearData()
{
    CardDispData* step=m_GameData.m_PlayData;
    CardDispData* stepnext=nullptr;
    while (step!=nullptr) {
        stepnext=step->getNextData();
        this->removeStep(step);
        step=stepnext;
    }
    if(!m_GameData.m_StepList.empty()&&m_GameData.m_StepList.size()>0)
        m_GameData.m_StepList.clear();
    m_GameData.m_PlayData=nullptr;
}

void CardDispManager::removeStep(CardDispData* data)
{
    this->removeStepFromList(data);
    CardDispData* stepnext=data->getNextData();
    CardDispData* stepprior=data->getPriorData();

	data->setNextData(nullptr);
	data->setPriorData(nullptr);
    //delete data;
	data = nullptr;

    if(stepnext!=nullptr)
    {
        stepnext->setPriorData(stepprior);
    }
    if (stepprior!=nullptr) {
        stepprior->setNextData(stepnext);
    }
}

void CardDispManager::removeStepFromList(CardDispData* data)
{
    StepList::iterator it=m_GameData.m_StepList.begin();
    while (it!=m_GameData.m_StepList.end()) {
        if((*it)==data)
        {
			(*it)->setNextData(nullptr);
			(*it)->setPriorData(nullptr);
			it = m_GameData.m_StepList.erase(it);
            break;
		}
		else
		{
			it++;
		}
    }
}

CardDispData* CardDispManager::resetLeafSetp(CardDispData* data)
{
    if (data==nullptr) {
        return nullptr;
    }
    CardDispData* next=data->getNextData();
    data->setNextData(nullptr);
    while (next!=nullptr) {
		CardDispData* ndata = next->getNextData();
        this->removeStep(next);
		next = ndata;
    }
    return data;
}

void CardDispManager::setMaxNumber(int64_t num)
{
    MaxScore* maxscore=nullptr;
    for (int i=0; i<m_ScoreList.size(); i++) {
        if (m_ScoreList[i]->m_SceneName.compare(m_CurrentScene)==0&&m_ScoreList[i]->m_Rows==m_DefRows) {
            maxscore=m_ScoreList[i];
        }
    }
    if (maxscore!=nullptr) {
        maxscore->m_MaxValue=num;
    }else
    {
        maxscore=new MaxScore();
        maxscore->m_SceneName=m_CurrentScene;
        maxscore->m_Rows=m_DefRows;
        maxscore->m_MaxValue=num;
        m_ScoreList.push_back(maxscore);
    }
}
int64_t CardDispManager::getMaxNumber()
{
    for (int i=0; i<m_ScoreList.size(); i++) {
        if (m_ScoreList[i]->m_SceneName.compare(m_CurrentScene)==0&&m_ScoreList[i]->m_Rows==m_DefRows) {
            return m_ScoreList[i]->m_MaxValue;
        }
    }
    return 0;
}

void CardDispManager::setMaxScore(int64_t num)
{
    MaxScore* maxscore=nullptr;
    for (int i=0; i<m_ScoreList.size(); i++) {
        if (m_ScoreList[i]->m_SceneName.compare(m_CurrentScene)==0&&m_ScoreList[i]->m_Rows==m_DefRows) {
            maxscore=m_ScoreList[i];
        }
    }
    if (maxscore!=nullptr) {
        maxscore->m_MaxScore=num;
    }else
    {
        maxscore=new MaxScore();
        maxscore->m_SceneName=m_CurrentScene;
        maxscore->m_Rows=m_DefRows;
        maxscore->m_MaxScore=num;
        m_ScoreList.push_back(maxscore);
    }
}

int64_t CardDispManager::getMaxScore()
{
    for (int i=0; i<m_ScoreList.size(); i++) {
        if (m_ScoreList[i]->m_SceneName.compare(m_CurrentScene)==0&&m_ScoreList[i]->m_Rows==m_DefRows) {
            return m_ScoreList[i]->m_MaxScore;
        }
    }
    return 0;
}

void CardDispManager::SetDefRows(int Rows)
{
    if (m_DefRows!=Rows) {
        this->freeTerrain();
        m_DefRows=Rows;
        //todo:reset score
		this->clearData();
        this->generateTerrain();
        this->writeConfig();
    }
}

int CardDispManager::GetDefRows()
{
    return m_DefRows;
}

void CardDispManager::setCurrentScene(string name)
{
    if (name.compare(m_CurrentScene)!=0) {
        //this->freeTerrain();
        m_CurrentScene=name;
        //todo:reset score
        this->writeConfig();
        //this->generateTerrain();
    }
}

string CardDispManager::getCurrentScene()
{
    return m_CurrentScene;
}

void CardDispManager::setIsSound(bool is)
{
    if(m_IsSound!=is)
    {
        m_IsSound = is;
        this->writeConfig();
    }
}

bool CardDispManager::getIsSound()
{
    return m_IsSound;
}

void CardDispManager::setPriorType(int is)
{
    m_PriorType=is;
    this->writeConfig();
}

int CardDispManager::getPriorType()
{
    return m_PriorType;
}

CardDispData* CardDispManager::GetRootStep()
{
    return  m_GameData.m_PlayData;
}

CardDispData* CardDispManager::GetRootStep(CardDispData* data)
{
    CardDispData* step=data;
    while (step!=nullptr&&step->getPriorData()!=nullptr) {
        step=step->getPriorData();
    }
    return step;
}

CardDispData* CardDispManager::GetLeafStep()
{
    return this->GetLeafStep(m_GameData.m_PlayData);
}

CardDispData* CardDispManager::GetLeafStep(CardDispData* data)
{
    CardDispData* step=data;
    while (step!=nullptr&&step->getNextData()!=nullptr) {
        step=step->getNextData();
    }
    if (step==nullptr) {
        step=m_GameData.m_PlayData;
    }
    return step;
}

CardDispData* CardDispManager::AddOneStep()
{
    CardDispData* data=new CardDispData(m_DefRows);
    m_GameData.m_StepList.push_back(data);
    if (m_GameData.m_PlayData==nullptr) {
        m_GameData.m_PlayData=data;
    }
    else
    {
        CardDispData* prior=this->GetLeafStep();
        prior->setNextData(data);
        data->setPriorData(prior);
    }
    return data;
}

/*
void CardDispManager::loadTransFile(string file)
{
    string names=FileUtils::getInstance()->getStringFromFile(file);
    string::size_type pos;
    string pattern=",";
    string words=names+pattern;//扩展字符串以方便操作
    int size=words.size();
    NameList namelist;
    for(int i=0; i<size; i++)
    {
        pos=words.find(pattern,i);
        if(pos<size)
        {
            std::string s=words.substr(i,pos-i);
            namelist.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    m_TransMap[file]=namelist;
}
*/

NameList* CardDispManager::getTransList(string type)
{
    if (m_TransMap.find(type)!=m_TransMap.end()) {
        return &m_TransMap[type]->m_List;
    }
    return nullptr;
}

NameList emptylist;

NameList* CardDispManager::getAntiTransList(string type)
{
	if (true)
	{
		if (m_TransMap.find(type) != m_TransMap.end()) {
			return &m_TransMap[type]->m_AntiList;
		}
	}
	return &emptylist;
}

void CardDispManager::loadTransJson(string file)
{
	string json_str = FileUtils::getInstance()->getStringFromFile(file);
	rapidjson::Document doc;
    doc.Parse<0>(json_str.c_str());
    
    rapidjson::Value& translist=doc;
    for (int k=0; k<translist.Size(); k++) {
        NameListObj* obj=new NameListObj();
        obj->m_name=translist[k]["name"].GetString();
        obj->m_unit=translist[k]["unit"].GetString();
        obj->m_max=translist[k]["max"].GetInt64();
		if (translist[k].HasMember("divide"))
		{
			obj->m_divide = translist[k]["divide"].GetInt();
		}
		else
		{
			obj->m_divide = 1;
		}
        rapidjson::Value& vals=translist[k]["list"];
        for(int i=0; i<vals.Size(); i++) {
            rapidjson::Value& num=vals[i];
            int j=0;
            TransNumber* number=new TransNumber();
            number->Intnum=num[j].GetInt64();
            number->transName=num[1].GetString();
            number->picture=num[2].GetString();
            number->Max=translist[k]["max"].GetInt64();
            number->color=num[3].GetString();
            number->goal=num[4].GetInt64();
			number->type = 0;
			if (num.Size() >= 6)
			{
				number->type = num[5].GetInt();
				if (num.Size() >= 7)
				{
					rapidjson::Value& alias = num[6];
					for (int j = 0; j < alias.Size(); j++)
					{
						number->aliasNames.push_back(alias[j].GetString());
					}
				}
			}
            obj->m_List.push_back(number);
        }
        if(translist[k].HasMember("anti_list"))
        {
            rapidjson::Value& avals=translist[k]["anti_list"];
            for(int i=0; i<avals.Size(); i++) {
                rapidjson::Value& num=avals[i];
                TransNumber* number=new TransNumber();
                number->Intnum=num[0].GetInt64();
                number->transName=num[1].GetString();
                number->picture=num[2].GetString();
                number->Max=translist[k]["max"].GetInt64();
                number->color=num[3].GetString();
				number->goal = num[4].GetInt64();
				number->type = 0;
				if (num.Size()>=6)
				{
					number->type = num[5].GetInt();
					if (num.Size() >= 7)
					{
						rapidjson::Value& alias = num[6];
						for (int j = 0; j < alias.Size(); j++)
						{
							number->aliasNames.push_back(alias[j].GetString());
						}
					}
				}
                obj->m_AntiList.push_back(number);
            }
        }
        m_TransMap[translist[k]["name"].GetString()]=obj;
    }
}

void CardDispManager::freeNameLists()
{
    MapNameList::iterator it=m_TransMap.begin();
    while (it!=m_TransMap.end()) {
        for(int i=0;i<(*it).second->m_List.size();i++)
        {
            delete (*it).second->m_List[i];
        }
        (*it).second->m_List.clear();
        for(int i=0;i<(*it).second->m_AntiList.size();i++)
        {
            delete (*it).second->m_AntiList[i];
        }
        (*it).second->m_AntiList.clear();
        delete (*it).second;
        it++;
    }
    m_TransMap.clear();
}

void CardDispManager::freeScoreList()
{
    for (int i=0; i<m_ScoreList.size(); i++) {
        delete  m_ScoreList[i];
    }
    m_ScoreList.clear();
}

string CardDispManager::getTransName(string name, int64_t num, int *aliasi)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
		char s[64] = {0};
        sprintf(s,"%lld",num);
        string str=string(s);
        return str;
    }
	std::string tname = "";
	bool isfound = false;
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
        long long modnum=m_TransMap[name]->m_List[i]->Intnum%(m_TransMap[name]->m_List[i]->Max*2);
        if (m_TransMap[name]->m_List[i]->Intnum==num||modnum==num) {
			if (m_TransMap[name]->m_List[i]->aliasNames.size() > 0)
			{
				int count = m_TransMap[name]->m_List[i]->aliasNames.size();
				int index = CardRandom() % count;
				if (*aliasi == -1)
					*aliasi = index;
				else if (*aliasi >= count)
					*aliasi = index;
				tname = m_TransMap[name]->m_List[i]->aliasNames[*aliasi];
			}
			else
			{
				tname=m_TransMap[name]->m_List[i]->transName;
			}
			isfound = true;
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
        long long modnum=m_TransMap[name]->m_AntiList[i]->Intnum%(m_TransMap[name]->m_AntiList[i]->Max*2);
        if (m_TransMap[name]->m_AntiList[i]->Intnum==num||modnum==num) {
			if (m_TransMap[name]->m_List[i]->aliasNames.size() > 0)
			{
				int count = m_TransMap[name]->m_List[i]->aliasNames.size();
				int index = CardRandom() % count;
				if (*aliasi == -1)
					*aliasi = index;
				else if (*aliasi >= count)
					*aliasi = index;
				tname = m_TransMap[name]->m_List[i]->aliasNames[*aliasi];
			}
			else
			{
				tname = m_TransMap[name]->m_AntiList[i]->transName;
			}
			isfound = true;
        }
    }
	if (!isfound)
	{
		char s[64] = {0};
		sprintf(s, "%lld", num);
		tname = string(s);
	}
	return tname;
}

string CardDispManager::getGoalStr(string name,int64_t num)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
		char s[64] = {0};
        sprintf(s,"%lld",num*2);
        string str=string(s);
        return str;
    }
	int index = -1;
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
        long long modnum=m_TransMap[name]->m_List[i]->Intnum%(m_TransMap[name]->m_List[i]->Max*2);
        if (m_TransMap[name]->m_List[i]->Intnum==num||modnum==num) {
			return this->getTransName(name, m_TransMap[name]->m_List[i]->goal, &index);
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
        long long modnum=m_TransMap[name]->m_AntiList[i]->Intnum%(m_TransMap[name]->m_AntiList[i]->Max*2);
        if (m_TransMap[name]->m_AntiList[i]->Intnum==num||modnum==num) {
			return this->getTransName(name, m_TransMap[name]->m_AntiList[i]->goal, &index);
        }
    }
	char s[64] = {0};
    sprintf(s,"%lld",num*2);
    string str=string(s);
    return str;
}


bool CardDispManager::isInList(vector<string>& list, string name)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].compare(name) == 0)
		{
			return true;
		}
	}
	return false;
}

string CardDispManager::getTransUnit(string name)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return "";
    }
    return m_TransMap[name]->m_unit;
}

bool CardDispManager::isTransValueExists(string name,int64_t num)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return false;
    }
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
        if (m_TransMap[name]->m_List[i]->Intnum==num) {
            return true;
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
        if (m_TransMap[name]->m_AntiList[i]->Intnum==num) {
            return true;
        }
    }
    return false;
}

int64_t CardDispManager::getTransValue(string name,string val)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return -1;
    }
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
		if (m_TransMap[name]->m_List[i]->transName.compare(val) == 0
		|| this->isInList(m_TransMap[name]->m_List[i]->aliasNames,val)) {
            return m_TransMap[name]->m_List[i]->Intnum;
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
		if (m_TransMap[name]->m_AntiList[i]->transName.compare(val) == 0 
		|| this->isInList(m_TransMap[name]->m_AntiList[i]->aliasNames, val)) {
            return m_TransMap[name]->m_AntiList[i]->Intnum;
        }
    }
    return -1;
}


TransNumber* CardDispManager::getNextNumber(string name,int64_t val)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return nullptr;
    }
    for(int i=0;i<m_TransMap[name]->m_List.size()-1;i++)
    {
        if (m_TransMap[name]->m_List[i]->Intnum==val) {
            return m_TransMap[name]->m_List[i+1];
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size()-1;i++)
    {
        if (m_TransMap[name]->m_AntiList[i]->Intnum==val) {
            return m_TransMap[name]->m_AntiList[i+1];
        }
    }
    return nullptr;
}

TransNumber* CardDispManager::getTransNumber(string name,int64_t num)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return nullptr;
    }
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
        if (m_TransMap[name]->m_List[i]->Intnum==num) {
            return m_TransMap[name]->m_List[i];
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
        if (m_TransMap[name]->m_AntiList[i]->Intnum==num) {
            return m_TransMap[name]->m_AntiList[i];
        }
    }
    return nullptr;
}

string CardDispManager::getTransPicture(string name,string val)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return "";
    }
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
		if (m_TransMap[name]->m_List[i]->transName.compare(val) == 0 
			|| this->isInList(m_TransMap[name]->m_List[i]->aliasNames, val)) {
            return m_TransMap[name]->m_List[i]->picture;
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
		if (m_TransMap[name]->m_AntiList[i]->transName.compare(val) == 0 
			|| this->isInList(m_TransMap[name]->m_List[i]->aliasNames, val)) {
            return m_TransMap[name]->m_AntiList[i]->picture;
        }
    }
    return "";
}

string CardDispManager::getTransPicture(string name,int64_t val)
{
    if(m_TransMap.find(name)==m_TransMap.end())
    {
        return "";
    }
    for(int i=0;i<m_TransMap[name]->m_List.size();i++)
    {
        if (m_TransMap[name]->m_List[i]->Intnum==val) {
            return m_TransMap[name]->m_List[i]->picture;
        }
    }
    for(int i=0;i<m_TransMap[name]->m_AntiList.size();i++)
    {
        if (m_TransMap[name]->m_AntiList[i]->Intnum==val) {
            return m_TransMap[name]->m_AntiList[i]->picture;
        }
    }
    return "";
}

void CardDispManager::generateTerrain()
{
	/*
    for (int i=0; i<GetDefRows()*GetDefRows(); i++) {
        TerrainElem* elem=new TerrainElem();
        elem->Pos.x=i/GetDefRows();
        elem->Pos.y=i%GetDefRows();
        elem->code = 0;
        m_GameData.m_Terrain.push_back(elem);
    }
	*/
}

void CardDispManager::freeTerrain()
{
	/*
    for (int i=0; i<m_GameData.m_Terrain.size(); i++) {
        delete m_GameData.m_Terrain[i];
    }
    m_GameData.m_Terrain.clear();
	*/
}



void CardDispManager::RePlaySteps()
{
    
}