#include "MarkerLayer.h"
#include "GeoUtils.h"
#include "SpriteWithHue.h"
#include "RadMath.h"
#include "RadLayer.h"
#include <iostream>
#include "AppData.h"
#include "BaseScene.h"
#include "CellScene.h"

int MarkerLayer::m_maxid = 0;

Marker* MarkerLayer::getMarker(int tag)
{
	for (int i = 0; i < m_marks.size(); i++)
	{
		if (m_marks[i]->m_tag == tag)
		{
			return m_marks[i];
		}
	}
	return nullptr;
}


void MarkerLayer::clearMarks()
{
	Vector<Node*> nodes = this->getChildren();
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes.at(i)->getTag() == 700157)
		{

		}
		else
		{
			nodes.at(i)->stopAllActions();
			nodes.at(i)->removeFromParentAndCleanup(true);
		}
	}
}


void MarkerLayer::selectMarks(Vec2 start, Vec2 end)
{
	Vec2 min(start);
	if (min.x > end.x)
	{
		min.x = end.x;
	}
	if (min.y > end.y)
	{
		min.y = end.y;
	}
	Size size(abs(start.x - end.x), abs(start.y - end.y));
	Rect rect(Vec2(min.x,min.y), size);
	Vector<Node*> nodes = this->getChildren();
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes.at(i)->getTag() == 700157 || nodes.at(i)->getTag() == 500001)
		{

		}
		else
		{
			SpriteWithHue* swh = (SpriteWithHue*)nodes.at(i);
			Marker* mark = (Marker*)swh->getUserData();
			if (mark != nullptr)
			{
				if (rect.containsPoint(swh->getPosition()))
				{
					mark->m_state = (mark->m_state + 1) % 2;
					swh->setSelect(mark->m_state == 1);
				}
				//CCLOG("type:%s", typeid(swh).name());
			}
			else
			{
				CCLOG("data is null %s %s %d", typeid(nodes.at(i)).name(), nodes.at(i)->getName().c_str(), nodes.at(i)->getTag());
			}
		}
	}
}

MarkerLayer::MarkerLayer(MapControl* pControl)
{
    m_pControl = pControl;
	m_iszooming = false;
	m_sel_type = 0;

	m_draw = DrawNode::create(2);
	this->addChild(m_draw,1314520);
	m_draw->setTag(700157);
	//std::function < void(int ozoom, int nzoom) >;
	
	//auto func = std::bind(&MarkerLayer::onZoom,this,std::placeholders::_1);
	//pControl->beforezooms["MarkerLayer"]=func;
	this->getEventDispatcher()->addCustomEventListener("mapZoom", std::bind(&MarkerLayer::onCustomEvent, this, std::placeholders::_1));
	this->getEventDispatcher()->addCustomEventListener("closeOper", std::bind(&MarkerLayer::onCustomEvent, this, std::placeholders::_1));
	this->getEventDispatcher()->addCustomEventListener("selMode", std::bind(&MarkerLayer::onCustomEvent, this, std::placeholders::_1));
	this->getEventDispatcher()->addCustomEventListener("lockMode", std::bind(&MarkerLayer::onCustomEvent, this, std::placeholders::_1));
	setTouchEnabled(true);

	this->scheduleOnce(std::bind(&MarkerLayer::onDelay, this, std::placeholders::_1), 0.6, "delay_init");
}

void MarkerLayer::onDelay(float delta)
{
	//119.30, 26.08 //fuzhou
	//121.27, 31.14 //shanghai
	
	std::string cnt = FileUtils::getInstance()->getStringFromFile("city/main_city.txt");
	std::vector<std::string> pos_name;

	this->str_split(cnt, "\r\n", &pos_name);

	for (int i = 2; i < pos_name.size(); i++)
	{
		this->AddMarker("geo/person.png", Vec2(121.27, 31.14), pos_name[i]);
		
	}
	
	std::vector<Func> m_funcs;
	Func func;
	func.m_name = "coin";
	func.m_pic = "city/attack.png";
	func.m_text = "硬币";
	func.m_scale = 0.5;
	func.m_tag = 250000 + 0;
	func.m_ref = -1;
	func.m_showtitle = false;
	func.m_select = 0;
	m_funcs.push_back(func);
	
	Func func1;
	func1.m_name = "wuxia";
	func1.m_pic = "city/attack.png";
	func1.m_text = "武侠";
	func1.m_scale = 0.5;
	func1.m_tag = 250000 + 1;
	func1.m_ref = -1;
	func1.m_showtitle = false;
	func1.m_select = 0;
	m_funcs.push_back(func1);

	Func func2;
	func2.m_name = "xiyou";
	func2.m_pic = "city/attack.png";
	func2.m_text = "西游";
	func2.m_scale = 0.5;
	func2.m_tag = 250000 + 2;
	func2.m_ref = -1;
	func2.m_showtitle = false;
	func2.m_select = 0;
	m_funcs.push_back(func2);

	Func func3;
	func3.m_name = "honglou";
	func3.m_pic = "city/attack.png";
	func3.m_text = "红楼";
	func3.m_scale = 0.5;
	func3.m_tag = 250000 + 3;
	func3.m_ref = -1;
	func3.m_showtitle = false;
	func3.m_select = 0;
	m_funcs.push_back(func3);

	Func func4;
	func4.m_name = "shuihu";
	func4.m_pic = "city/attack.png";
	func4.m_text = "水浒";
	func4.m_scale = 0.5;
	func4.m_tag = 250000 + 4;
	func4.m_ref = -1;
	func4.m_showtitle = false;
	func4.m_select = 0;
	m_funcs.push_back(func4);

	Func func5;
	func5.m_name = "3king";
	func5.m_pic = "city/attack.png";
	func5.m_text = "三国";
	func5.m_scale = 0.5;
	func5.m_tag = 250000 + 5;
	func5.m_ref = -1;
	func5.m_showtitle = false;
	func5.m_select = 0;
	m_funcs.push_back(func5);

	Func func6;
	func6.m_name = "127";
	func6.m_pic = "city/attack.png";
	func6.m_text = "127";
	func6.m_scale = 0.5;
	func6.m_tag = 250000 + 6;
	func6.m_ref = -1;
	func6.m_showtitle = false;
	func6.m_select = 0;
	m_funcs.push_back(func6);
	
	this->addOperates("geo/oper_person.png", Vec2(116.10, 26.08),200000,false, pos_name[0], m_funcs, std::bind(&MarkerLayer::onClickOperate, this, std::placeholders::_1
		, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

	this->refreshMarker();
	this->unschedule("delay_init");
}


void MarkerLayer::setCurrZoom(int zoom)
{
	m_currZoom = zoom;
}

void MarkerLayer::OneTree()
{
	Size size = Director::getInstance()->getVisibleSize();
	SpriteWithHue* sp = SpriteWithHue::create("geo/person.png");
	Vec3 center = Vec3(size.width / 2, size.height / 2, 1.0);
	RadTree* tree = new RadTree(center, sp->getContentSize(), 2, 5);
	std::vector<Vec3> list;
	tree->GetAllWidgetPos(&list);
	for (int i = 0; i < list.size(); i++)
	{
		SpriteWithHue* sp = SpriteWithHue::create("geo/person.png");
		this->addChild(sp);
		Vec2 p(list[i].x, list[i].y);
		sp->setPosition(p);
	}
}


void MarkerLayer::setSelType(int atype)
{
	m_sel_type = atype;
}

void MarkerLayer::onCustomEvent(EventCustom* cevent)
{
	if (cevent->getEventName().compare("mapZoom") == 0)
	{
		this->DelMarker(this->getMarker(500001));
		if (cevent->getUserData() != nullptr)
		{
			std::string msg((char*)cevent->getUserData());
			//CCLOG("mapZoom=%s", cevent->getUserData());
			if (msg.compare("zoomIn")==0)
			{
				m_pControl->zoomIn();
			}
			else if (msg.compare("zoomOut") == 0)
			{
				m_pControl->zoomOut();
			}
			this->m_iszooming = true;
			if (!this->isScheduled("delay_del"))
			{
				this->scheduleOnce([this](float delta)->void{
					this->m_iszooming = false;
					this->DelMarker(this->getMarker(500001));
					this->unschedule("delay_del");
				}, 0.15, "delay_del");
			}
		}
		this->refreshMarker();
	}
	else if (cevent->getEventName().compare("closeOper") == 0)
	{
		this->DelMarker(this->getMarker(500001));
	}
	else if (cevent->getEventName().compare("selMode") == 0)
	{
		std::string msg((char*)cevent->getUserData());
		this->m_sel_type = atoi(msg.c_str());
	}
}


bool MarkerLayer::isInSelect(Marker* mark)
{
	for (int i = 0; i < m_selects.size(); i++)
	{
		if (m_selects[i] == mark)
		{
			return true;
		}
	}
	return false;
}

MarkerLayer::~MarkerLayer()
{
	//std::map<std::string, MapCtlFunc>::iterator it = m_pControl->beforezooms.find("MarkerLayer");
	//m_pControl->beforezooms.erase(it);
	this->getEventDispatcher()->removeCustomEventListeners("mapZoom");
	this->getEventDispatcher()->removeCustomEventListeners("selMode");
	this->getEventDispatcher()->removeCustomEventListeners("closeOper");
	for (int i = 0; i < m_marks.size(); i++)
	{
		if (m_marks[i]->m_funcs.size() > 0)
		{
			for (int j = 0; j < m_marks[i]->m_funcs.size(); j++)
			{
				delete m_marks[i]->m_funcs[j];
			}
		}
		m_marks[i]->m_funcs.clear();
		delete m_marks[i];
	}
	m_marks.clear();
}


bool MarkerLayer::onClickSprite(Sprite* ahue, int atype, Touch * atouch, Event * aevent)
{
	SpriteWithHue* sp = (SpriteWithHue*)ahue;
	Marker* mark = (Marker*)sp->getUserData();
	mark->m_pt_m = atouch->getLocation();
	int tag = ahue->getTag();
	if (atype == 0)
	{
		if (mark->m_type == 1)
		{
			if (mark->m_callback != nullptr)
			{
				Func* func = nullptr;
				int tag = ahue->getTag();
				if (tag >= 250000&&tag<300000)
				{
					int index = tag - 250000;
					if (mark->m_funcs.size() > index)
					{
						func = mark->m_funcs[index];
					}
					std::string data = "cancel_inherite";
					aevent->setEventData((void*)data.c_str(), false, data.length());
				}
				else if (tag >= 300000 && tag < 400000)
				{
					int index = tag - 300000;
					if (mark->m_funcs.size() > index)
					{
						func = mark->m_funcs[index];
					}
					std::string data = "cancel_inherite";
					aevent->setEventData((void*)data.c_str(), false, data.length());
					bool ret=mark->m_callback(mark, func, ahue, atype, atouch, aevent);
					return ret;
				}
				else if (tag == 500001)
				{
					std::string data = "cancel_inherite";
					aevent->setEventData((void*)data.c_str(), false, data.length());
				}
			}
		}
		if (tag >= 150000 && tag<250000||tag==500001)
		{
			std::string data = "cancel_inherite";
			aevent->setEventData((void*)data.c_str(), false, data.length());
			//CCLOG("TODO:collect coin");
			mark->m_showtitle = false;
		}
		else if (tag >= 250000 && tag < 300000)
		{

		}
		else if (tag >= 350000 && tag < 400000)
		{
			std::string data = "cancel_inherite";
			aevent->setEventData((void*)data.c_str(), false, data.length());
			//CCLOG("TODO:collect coin");
			//mark->m_showtitle = false;
			Func* func = (Func*)mark;
			if (func != nullptr)
			{
				func->m_showtitle = false;
				Marker* imark = this->getMarker(func->m_ref);
				if (imark != nullptr)
				{
					imark->m_showtitle = false;
				}
			}
		}
		else
		{
			if (mark->m_select == 1)
			{
				std::string data = "cancel_inherite";
				aevent->setEventData((void*)data.c_str(), false, data.length());
			}
			else
			{
				if (aevent->getEventData() != nullptr)
				{
					mark->m_msg = std::string((char*)aevent->getEventData());
					if (mark->m_msg.compare("cancel_inherite") == 0)
					{
						return false;
					}
				}
			}
		}
	}
	else if (atype == 2)
	{
		if (mark->m_type == 0)
		{
			if (tag < 150000)
			{
				if (!this->isInSelect(mark))
				{
					m_selects.push_back(mark);
				}
				if (m_selects.size() > 0&&!this->isScheduled("delay_oper"))
				{
					this->scheduleOnce([this](float delta)->void{
						if (!this->m_iszooming)
						{
							Marker* mark = m_selects[0];
							if (m_selects.size() == 1)
							{
								mark->m_state = (mark->m_state + 1) % 2;
								SpriteWithHue* sp = (SpriteWithHue*)mark->m_sp;
								sp->setSelect(mark->m_state == 1);
								//for test
								//Vec2 p = GeoUtils::toTileXY(mark->m_pos.y, mark->m_pos.x, m_pControl->getZoomLevel());
								//CCLOG("click mark %s lat=%f,lon=%f,,,tilexy=%f:%f", mark->m_title.c_str(), mark->m_pos.x, mark->m_pos.y,p.x,p.y);
								//this->getLatLon(m_tBeginPos);

								if (!mark->m_showtitle)
								{
									mark->m_title_pic = this->randomPic();
									mark->m_showtitle = true;
								}
							}
							else
							{
								Vec3 center = Vec3(mark->m_pt_m.x, mark->m_pt_m.y, 1.0);
								Size size(mark->m_sp->getContentSize().width, mark->m_sp->getContentSize().height);
								float radius = sqrt(size.width*size.width + size.height*size.height);
								RadCircle* circle = new RadCircle(center, size.width);
								std::vector<Vec3> m_poss;

								if (m_sel_type == 1||m_sel_type==4)
								{
									//for select more than one onetimes
									circle->GetWidgetsPos(nullptr, &m_poss, size, m_selects.size() + 1);
									std::vector<Func> m_funcs;
									for (int i = 0; i < m_selects.size() + 1; i++)
									{
										Vec2 p(m_poss[i].x, m_poss[i].y);
										Func func;
										func.m_tag = 300000 + i;
										if (i == m_selects.size())
										{
											func.m_name = "sel_all";
											func.m_pic = "heart/heart02.png";
											func.m_text = "关闭";
											func.m_scale = 0.8;
											func.m_state = 0;
											func.m_select = 0;
											func.m_showtitle = false;
											func.m_select = true;
											func.m_ref = -1;
										}
										else
										{
											func.m_name = m_selects[i]->m_name;
											func.m_pic = m_selects[i]->m_pic;
											func.m_text = m_selects[i]->m_title;
											func.m_scale = m_selects[i]->m_scale;
											func.m_state = m_selects[i]->m_state;
											func.m_select = m_selects[i]->m_select;
											func.m_ref = m_selects[i]->m_tag;
											func.m_showtitle = m_selects[i]->m_showtitle;
											func.m_title_pic = m_selects[i]->m_title_pic;
											func.m_title = m_selects[i]->m_title;
											func.m_select = true;
										}
										m_funcs.push_back(func);
									}
									this->DelMarker(this->getMarker(500001));
									this->addOperates("heart/heart01.png", mark->m_pt_m, 500001, true, "", m_funcs, std::bind(&MarkerLayer::onClickOperate, this, std::placeholders::_1
										, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
									this->refreshMarker();
								}
								else if (m_sel_type == 0 || m_sel_type == 3)
								{
									//for select one onetimes
									circle->GetWidgetsPos(nullptr, &m_poss, size, m_selects.size());
									SpriteWithHue* sp = SpriteWithHue::create("heart/heart01.png");
									sp->setScale(0.5);
									this->addChild(sp);
									sp->setPosition(mark->m_pt_m);
									//CCLOG("Size:%d", m_poss.size());
									for (int i = 0; i < m_selects.size(); i++)
									{
										Vec2 p(m_poss[i].x, m_poss[i].y);
										m_selects[i]->m_sp->setPosition(p);
										SpriteWithHue* sp = (SpriteWithHue*)m_selects[i]->m_sp;
										sp->setHue(sp->getHue() + 1.5*rad_pi);
										sp->setLocalZOrder(99);
										m_selects[i]->m_select = 1;
									}
								}
								delete circle;
							}
							m_selects.clear();
						}
						this->unschedule("delay_oper");
					}, 0.1, "delay_oper");
				}
			}
		}
		else if (mark->m_type == 1)
		{
			if (mark->m_callback != nullptr)
			{
				Func* func = nullptr;
				
				if (tag >= 250000 && tag<300000 && tag != 500001)
				{
					int index = tag - 250000;
					if (mark->m_funcs.size() > index)
					{
						func = mark->m_funcs[index];
					}
				}
				else if (tag >= 300000 && tag<350000 && tag != 500001)
				{
					int index = tag - 300000;
					if (mark->m_funcs.size() > index)
					{
						func = mark->m_funcs[index];
					}
				}
				else
				{
					if (!mark->m_transition)
					{
						mark->m_showoper = !mark->m_showoper;
						if (mark->m_showoper)
						{
							this->showOperateSprite(mark);
						}
						else
						{
							this->closeOperateSprite(mark);
						}
					}
				}
				mark->m_callback(mark, func, ahue, atype, atouch, aevent);
			}
		}
		
	}
	return true;
}

bool MarkerLayer::onClickOperate(Marker* mark, Func* func, Sprite* ahue, int atype, Touch * atouch, Event * aevent)
{
	if (atype == 2)
	{
		std::string name = "";
		if (func != nullptr){ 
			name = func->m_name;
			if (func->m_tag >= 300000 && func->m_tag < 400000)
			{
				if (func->m_name.compare("sel_close") == 0)
				{
					this->DelMarker(mark);
				}
				else if (func->m_name.compare("sel_all") == 0)
				{
					for (int i = 0; i < mark->m_funcs.size()-1; i++)
					{
						Func* func = mark->m_funcs[i];
						func->m_state = (func->m_state + 1) % 2;
						SpriteWithHue* sp = (SpriteWithHue*)mark->m_parent->getChildByTag(func->m_tag);
						sp->setSelect(func->m_state == 1);
						Marker* ref = this->getMarker(func->m_ref);
						if (ref != nullptr)
						{
							ref->m_state = (ref->m_state + 1) % 2;
							SpriteWithHue* spr = (SpriteWithHue*)ref->m_sp;
							spr->setSelect(ref->m_state == 1);
						}
					}
				}
				else
				{
					func->m_state = (func->m_state + 1) % 2;
					SpriteWithHue* sp = (SpriteWithHue*)mark->m_parent->getChildByTag(func->m_tag);
					sp->setSelect(func->m_state==1);
					Marker* ref = this->getMarker(func->m_ref);
					if (!func->m_showtitle)
					{
						func->m_title_pic = this->randomPic();
						func->m_title = "";
						func->m_showtitle = true;
					}
					if (ref != nullptr)
					{
						ref->m_state = (ref->m_state + 1) % 2;
						SpriteWithHue* spr = (SpriteWithHue*)ref->m_sp;
						spr->setSelect(ref->m_state==1);
						ref->m_showtitle = func->m_showtitle;
						ref->m_title = func->m_title;
						ref->m_title_pic = func->m_title_pic;
					}
				}
			}
			else if (func->m_tag >= 250000 && func->m_tag < 300000)
			{
				CCLOG("click %s", func->m_name.c_str());
				if (func->m_name.compare("honglou") == 0)
				{
					//TODO:
				}
				else if (func->m_name.compare("shuihu") == 0)
				{
					//TODO:
				}
				else if (func->m_name.compare("xiyou") == 0)
				{
					//TODO:
				}else if (func->m_name.compare("wuxia") == 0)
				{
					//TODO:
				}
				else if (func->m_name.compare("3king") == 0)
				{
					//TODO:
				}
				else if (func->m_name.compare("coin") == 0)
				{
					Director::getInstance()->pushScene(BaseScene::createTNScene(0.5, SceneMoneyInBa5::create()));
				}
				else if (func->m_name.compare("127") == 0)
				{
					Director::getInstance()->pushScene(BaseScene::createTNScene(0.5, SceneMoneyIb127::create()));
				}
			}
		}
		else
		{
			if (mark->m_tag == 500001)
			{
				this->DelMarker(mark);
			}
		}
	}
	return true;
}


void MarkerLayer::AddMarker(std::string strImage, Vec2 pnt, std::string param)
{   
	std::vector<std::string> params;
	m_maxid = m_maxid + 1;
	if (m_maxid > 200000){ m_maxid = 0; }
	this->str_split(param, "\t", &params);
	if (params[2].compare("中国") == 0 
		|| params[2].compare("新加坡") == 0
		|| params[2].compare("美国")==0)
	{
		Marker* mark = new Marker();
		mark->m_pos = pnt;
		mark->m_state = 0;
		mark->m_select = 0;
		mark->m_pic = strImage;
		mark->m_tag = 100000 + m_maxid;
		mark->m_type = 0;
		mark->m_scale = 1.2;
		mark->m_callback = nullptr;
		mark->m_showoper = false;
		mark->m_showtitle = true;
		mark->m_cn_name = params[1];
		mark->m_en_name = params[0];
		mark->m_sections_name = params[2];
		mark->m_ct_type = atoi(params[3].c_str());
		Vec2 pos;
		std::vector<std::string> lon;
		this->str_split(params[4], ":", &lon);
		std::vector<std::string> lat;
		this->str_split(params[5], ":", &lat);
		if (lon[0].compare("北纬") == 0){
			pos.y = atof(lon[1].c_str());
		}
		else
		{
			pos.y = -atof(lon[1].c_str());
		}
		if (lat[0].compare("东经") == 0){
			pos.x = atof(lat[1].c_str());
		}
		else
		{
			pos.x = -atof(lat[1].c_str());
		}
		mark->m_pos = pos;
		mark->m_title_pic = this->randomPic();
		mark->m_title = "";
		m_marks.push_back(mark);
	}
}


void MarkerLayer::DelMarker(Marker* mark)
{
	if (mark != nullptr)
	{
		std::vector<Marker*>::iterator it = m_marks.begin();
		while (it != m_marks.end())
		{
			if ((*it) == mark)
			{
				m_marks.erase(it);
				delete mark;
				break;
			}
			it++;
		}
	}
	m_selects.clear();
}

void MarkerLayer::str_split(std::string cnt, std::string delim, std::vector<std::string> *citys)
{
	size_t last = 0;
	size_t index = cnt.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		std::string str = cnt.substr(last, index - last);
		if (str.compare("") != 0)
		{
			citys->push_back(str);
		}
		last = index + 1;
		index = cnt.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		std::string str = cnt.substr(last, index - last);
		if (str.compare("") != 0)
		{
			citys->push_back(str);
		}
	}
}

void MarkerLayer::addOperates(std::string strImage, Vec2 pnt, int tag,bool show, std::string params, std::vector<Func> operates, MarkCallBack cb)
{
	m_maxid = m_maxid + 1;
	Marker* mark = new Marker();
	mark->m_pos = pnt;
	mark->m_pt = pnt;
	mark->m_state = 0;
	mark->m_pic = strImage;
	mark->m_tag = tag;
	mark->m_type = 1;
	mark->m_scale = 1.0;
	mark->m_showoper = show;
	mark->m_transition = false;
	for (int i = 0; i < operates.size(); i++)
	{
		Func* func=new Func();
		func->m_name = std::string(operates[i].m_name);
		func->m_pic = std::string(operates[i].m_pic);
		func->m_pos = Vec2(operates[i].m_pos);
		func->m_text = std::string(operates[i].m_text);
		func->m_scale = operates[i].m_scale;
		func->m_tag = operates[i].m_tag;
		func->m_state = operates[i].m_state;
		func->m_select = operates[i].m_select;
		func->m_ref = operates[i].m_ref;
		func->m_title = operates[i].m_title;
		func->m_title_pic = operates[i].m_title_pic;
		func->m_showtitle = operates[i].m_showtitle;
		mark->m_funcs.push_back(func);
	}
	mark->m_callback = cb;
	m_marks.push_back(mark);
}

cocos2d::Vec2 MarkerLayer::dispCenter(Vec2 center, float radius, Size size, Size wsize)
{
	if (center.x + radius + wsize.width/2 > size.width)
	{
		center.x = size.width - radius-wsize.width/2;
	}
	else if (center.x - radius -wsize.width/2 < 0)
	{
		center.x = radius+wsize.width/2;
	}
	if (center.y + radius+wsize.height/2>size.height)
	{
		center.y = size.height - radius-wsize.height/2;
	}
	else if (center.y - radius-wsize.height/2 < 0)
	{
		center.y = radius-wsize.height/2;
	}
	return center;
}



void MarkerLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent)
{
	//CCLOG("MarkerLayer::onTouchesBegan");
	Touch* touch = touches[0];
	m_tBeginPos = touch->getLocation();
	//this->getLatLon(m_tBeginPos);
}

void MarkerLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent)
{
	int sel = AppSetting::getSingleton().getSelMode();
	if (sel == 2 || sel == 3 || sel == 4)
	{
		m_draw->clear();
		m_draw->drawRect(m_tBeginPos, touches[0]->getLocation(), Color4F::RED);
	}
	if (this->isEatMsg(aevent))
	{
		return;
	}
	this->DelMarker(this->getMarker(500001));
	Touch* touch = touches[0];
	Vec2 touchLocation = touch->getLocation();
	Ref* child;
	for each(child in getChildren())
	{
		Node* pNode = (Node*)child;
		ActionInterval* actionBy = MoveBy::create(0.1, Vec2(touchLocation.x - m_tBeginPos.x, touchLocation.y - m_tBeginPos.y));
		pNode->runAction(actionBy);
	}
	m_tBeginPos = touchLocation;
}

void MarkerLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent)
{
	if (m_sel_type == 2 || m_sel_type == 3 || m_sel_type == 4)
	{
		m_draw->clear();
		this->selectMarks(m_tBeginPos, touches[0]->getLocation());
	}
	this->refreshMarker();
}

Vec2 MarkerLayer::getLatLon(Vec2 pos)
{
	//TODO:通过屏幕坐标计算经纬度
	Size size = Director::getInstance()->getVisibleSize();
	RawTile dTile = m_pControl->getMap()->m_defTile;
	Vec2 pntOffset = m_pControl->getMap()->getGlobalOffset();

	Vec2 pntTopLeft, pntRightBottom, posTile, posOffset, posWorld;
	pntTopLeft.x = dTile.x * 256 - pntOffset.x;
	pntTopLeft.y = dTile.y * 256 - pntOffset.y;

	pntRightBottom.x = pntTopLeft.x + m_pControl->getMap()->getWidth();
	pntRightBottom.y = pntTopLeft.y + m_pControl->getMap()->getHeight();

	CCLOG("%f:%f::%f:%f", pntTopLeft.x,pntTopLeft.y,pntRightBottom.x,pntRightBottom.y);

	posWorld.x = pntTopLeft.x + pos.x;
	posWorld.y = pntTopLeft.y + size.height-pos.y;

	posTile.x = floor(posWorld.x / 256);
	posTile.y = floor(posWorld.y / 256);
	posOffset.x = int(posWorld.x) % 256;
	posOffset.y = int(posWorld.y) % 256;

	Vec2 offsetGps = m_pControl->getGpsOffset();
	int zoom = m_pControl->getZoomLevel();
	double latFix_offset = offsetGps.y*pow(10.0, -5);
	double lonFix_offset = offsetGps.x*pow(10.0, -5);

	//Vec2 p = GeoUtils::getLatLong(posWorld.x, posWorld.y, zoom);
	GeoConvert convert;
	Vec2 p = convert.ToLatLng(posWorld, zoom);

	CCLOG("gpsoffset=%f:%f,pos=%f:%f,posworld=%f:%f,lat=%f,lon=%f,offset=%f:%f", offsetGps.x, offsetGps.y, pos.x, pos.y, posWorld.x, posWorld.y, p.x + latFix_offset, p.y + lonFix_offset, latFix_offset, lonFix_offset);

	/*
	Vec2 p = GeoUtils::toTileXY(latFix, lonFix, zoom);
	Vec2 off = GeoUtils::getPixelOffsetInTile(latFix, lonFix, zoom);
	Vec2 pp(pntMarker.x - pntTopLeft.x, m_pControl->getMap()->getHeight() - (pntMarker.y - pntTopLeft.y));
	*/

	return pos;
}

void MarkerLayer::refreshMarker()
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([this](void)->void{
		//TODO:error of removeAllChildren();
		//https://www.cnblogs.com/titer1/archive/2011/12/19/2293650.html
		//removeAllChildren();
		clearMarks();
		int nCount = m_marks.size();
		for (int i = 0; i < nCount; ++i)
		{
			Marker* pnt = m_marks[i];
			if (pnt->m_tag == 500001)
			{
				this->addOperateSprite(pnt);
			}
			else
			{
				if (m_pControl) {
					double lat = pnt->m_pos.y;
					double lon = pnt->m_pos.x;

					Vec2 offsetGps = m_pControl->getGpsOffset();
					int zoom = 17 - m_pControl->getZoomLevel();
					double latFix = lat + offsetGps.y*pow(10.0, -5);
					double lonFix = lon + offsetGps.x*pow(10.0, -5);

					RawTile dTile = m_pControl->getMap()->m_defTile;
					Vec2 pntOffset = m_pControl->getMap()->getGlobalOffset();

					Vec2 pntTopLeft, pntRightBottom, pntMarker;
					pntTopLeft.x = dTile.x * 256 - pntOffset.x;
					pntTopLeft.y = dTile.y * 256 - pntOffset.y;
					pntRightBottom.x = pntTopLeft.x + m_pControl->getMap()->getWidth();
					pntRightBottom.y = pntTopLeft.y + m_pControl->getMap()->getHeight();

					Vec2 p = GeoUtils::toTileXY(latFix, lonFix, zoom);
					Vec2 off = GeoUtils::getPixelOffsetInTile(latFix, lonFix, zoom);

					pntMarker.x = (p.x) * 256 + off.x;
					pntMarker.y = (p.y) * 256 + off.y;

					//for debug
					//if (pnt->m_cn_name.compare("福州") == 0)
					//{
					//	m_temppos = pntMarker;
					//}

					if (pntMarker.x > pntTopLeft.x && pntMarker.y > pntTopLeft.y && pntMarker.x < pntRightBottom.x && pntMarker.y < pntRightBottom.y) {
						Vec2 pp(pntMarker.x - pntTopLeft.x, m_pControl->getMap()->getHeight() - (pntMarker.y - pntTopLeft.y));
						pnt->m_pt = pp;
						if (pnt->m_type == 0)
						{
							this->addFlagSprite(pnt);
						}
						else if (pnt->m_type == 1)
						{
							this->addOperateSprite(pnt);
						}
						else
						{
							CCLOG("TODO:add mark type for %d", pnt->m_type);
						}
					}
				}
			}
		}
	});
}

void MarkerLayer::addFlagSprite(Marker* mark)
{
	SpriteWithHue* hue = (SpriteWithHue*)this->getChildByTag(mark->m_tag);
	mark->m_select = 0;
	if (hue == nullptr)
	{
		hue = SpriteWithHue::create("geo/person.png");
		hue->setEnable(true);
		hue->setSelect(mark->m_state == 1);
		hue->setTag(mark->m_tag);
		hue->setUserData(mark);
		hue->setName(mark->m_name);
		hue->setScale(mark->m_scale);
		mark->m_sp = hue;
		//hue->setDefineInherite(true);
		hue->setCallBack(std::bind(&MarkerLayer::onClickSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		this->addChild(hue);
	}
	hue->setLocalZOrder(0);
	hue->setPosition(Vec2(mark->m_pt.x, mark->m_pt.y));
	this->showTitle(mark);
}

void MarkerLayer::addOperateSprite(Marker* mark)
{
	SpriteWithHue* hue = (SpriteWithHue*)this->getChildByTag(mark->m_tag);
	Size size = Director::getInstance()->getVisibleSize();
	if (hue == nullptr)
	{
		hue = SpriteWithHue::create(mark->m_pic);
		hue->setSelect(mark->m_state == 1);
		hue->setTag(mark->m_tag);
		hue->setUserData(mark);
		hue->setName(mark->m_name);
		hue->setScale(mark->m_scale);
		hue->setDefineInherite(true);
		hue->setCallBack(std::bind(&MarkerLayer::onClickSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		if (mark->m_tag == 500001)
		{
			RadLayer* layer = (RadLayer*)this->getChildByName("mask");
			if (layer == nullptr)
			{
				layer=RadLayer::create(Color4B::BLACK, size.width * 4, size.height * 4);
				layer->setOpacity(160);
				this->addChild(layer);
				layer->setCancelEat(true);
				layer->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
				layer->setTouchEnabled(true);
				layer->setSwallowsTouches(true);
				layer->setName("mask");
			}
			Vec2 p(-size.width, -size.height);
			layer->setPosition(p);
			layer->addChild(hue);
			mark->m_sp = hue;
			mark->m_parent = layer;
			this->showOperateSprite(mark);
		}
		else
		{
			mark->m_sp = hue;
			mark->m_parent = this;
			this->addChild(hue);
			this->showOperateSprite(mark);
		}
	}
	hue->setEnable(true);
	if (mark->m_tag == 500001)
	{
		hue->setHue(hue->getHue() + 1.5*rad_pi);
		hue->setPosition(Vec2(mark->m_pt.x + size.width, mark->m_pt.y + size.height));
	}
	else
	{
		hue->setPosition(Vec2(mark->m_pt.x, mark->m_pt.y));
	}	
}

void MarkerLayer::showOperateSprite(Marker* mark)
{
	if (mark->m_funcs.size() > 0 && mark->m_showoper)
	{
		SpriteWithHue* thue = SpriteWithHue::create(mark->m_funcs[0]->m_pic);
		Size size(thue->getContentSize().width*mark->m_funcs[0]->m_scale, thue->getContentSize().height*mark->m_funcs[0]->m_scale);
		Vec3 cener(mark->m_pt.x, mark->m_pt.y, 1);
		RadCircle* circle = new RadCircle(cener, size.width);
		std::vector<Vec3> m_list;
		circle->GetWidgetsPos(nullptr, &m_list, size, mark->m_funcs.size());
		for (int i = 0; i < mark->m_funcs.size(); i++)
		{
			Func* func = mark->m_funcs[i];
			func->m_pos = Vec2(m_list[i].x,m_list[i].y);
			SpriteWithHue* ohue = (SpriteWithHue*)this->getChildByTag(func->m_tag);
			if (ohue == nullptr)
			{
				ohue = SpriteWithHue::create(func->m_pic);
				ohue->setTag(func->m_tag);
				ohue->setUserData(mark);
				ohue->setName(func->m_name + "_oper");
				ohue->setScale(func->m_scale);
				ohue->setDefineInherite(true);
				ohue->setCallBack(std::bind(&MarkerLayer::onClickSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
				Node* pnode = mark->m_parent;
				if (pnode == nullptr)
				{
					pnode = this;
				}
				pnode->addChild(ohue);
			}
			ohue->setEnable(true);
			ohue->setSelect(func->m_state == 1);
			if (func->m_select==1)
			{
				ohue->setHue(ohue->getHue() + 1.5*rad_pi);
			}
			if (mark->m_tag == 500001)
			{
				Size s = Director::getInstance()->getVisibleSize();
				Vec2 p(func->m_pos.x + s.width, func->m_pos.y + s.height);
				ohue->setPosition(p);
			}
			else
			{
				ohue->setPosition(func->m_pos);
			}
			func->m_sp = ohue;
			this->showTitle(func);
			//EaseBounceOut* ee = EaseBounceOut::create(MoveTo::create(0.7, func->m_pos));
			//ohue->runAction(Sequence::create(Show::create(),ee, CallFunc::create([&ohue, &mark]()->void{
			//	mark->m_showoper = true;
			//	mark->m_transition = false;
			//}),nullptr));
		}
		delete circle;
		//mark->m_transition = true;
	}
}

void MarkerLayer::closeOperateSprite(Marker* mark)
{
	if (mark->m_funcs.size() > 0)
	{
		for (int i = 0; i < mark->m_funcs.size(); i++)
		{
			SpriteWithHue* sp = (SpriteWithHue*)this->getChildByTag(mark->m_funcs[i]->m_tag);
			if (sp != nullptr)
			{
				sp->setEnable(false);
				sp->removeFromParentAndCleanup(true);
				//sp->runAction(Sequence::create(EaseBounceIn::create(MoveTo::create(0.7, mark->m_pt)), CallFunc::create([&sp, &mark]()->void {
				//	mark->m_showoper = false;
				//	mark->m_transition = false;
				//}), RemoveSelf::create(), nullptr));
			}
		}
	}
	//mark->m_transition = true;
}

void MarkerLayer::showTitle(Marker* mark)
{
	if (mark->m_showtitle)
	{
		SpriteWithHue* m_hue = SpriteWithHue::create(mark->m_title_pic);
		float scale = 0.2;
		m_hue->setScale(scale);
		m_hue->setName(mark->m_name + "_head");
		m_hue->setEnable(true);
		Size size = mark->m_sp->getContentSize();
		Size si = m_hue->getContentSize();
		mark->m_sp->addChild(m_hue);
		m_hue->setUserData(mark);
		m_hue->setName("title");
		m_hue->setDefineInherite(true);
		m_hue->setTag(mark->m_tag + 50000);
		m_hue->setCallBack(std::bind(&MarkerLayer::onClickSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_hue->setPosition(Vec2(size.width / 2, size.height + si.height* scale / 3));
		RepeatForever* rep = RepeatForever::create(OrbitCamera::create(1.2, 64, 0, 0, 360, 0, 0));
		m_hue->runAction(rep);
	}
}

void MarkerLayer::showTitle(Func* func)
{
	if (func->m_showtitle&&FileUtils::getInstance()->isFileExist(func->m_title_pic))
	{
		SpriteWithHue* m_hue = SpriteWithHue::create(func->m_title_pic);
		float scale = 0.2;
		m_hue->setScale(scale);
		m_hue->setName(func->m_name + "_head");
		m_hue->setEnable(true);
		Size size = func->m_sp->getContentSize();
		Size si = m_hue->getContentSize();
		func->m_sp->addChild(m_hue);
		m_hue->setUserData(func);
		m_hue->setDefineInherite(true);
		m_hue->setTag(func->m_tag + 50000);
		m_hue->setName("title");
		m_hue->setCallBack(std::bind(&MarkerLayer::onClickSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		m_hue->setPosition(Vec2(size.width / 2, size.height + si.height* scale / 3));
		RepeatForever* rep = RepeatForever::create(OrbitCamera::create(1.2, 64, 0, 0, 360, 0, 0));
		m_hue->runAction(rep);
	}
}

std::string MarkerLayer::randomPic()
{
	std::string pic = "";
	std::vector<std::string> coins;
	coins.push_back("coin/1fen.png");
	coins.push_back("coin/2fen.png");
	coins.push_back("coin/5fen.png");
	coins.push_back("coin/1jiao.png");
	coins.push_back("coin/2jiao.png");
	coins.push_back("coin/5jiao.png");
	coins.push_back("coin/1yuan.png");
	coins.push_back("city/jini.png");
	int r = random(1, 10);
	if (r > 7){ 
		pic = "heart/heart2.png"; 
	}else {
		int index = random(1, (int)coins.size())-1;
		pic = coins[index];
	}
	return pic;
}


