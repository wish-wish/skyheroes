#include "PublicMsgHandler.h"
#include <thread>
#include "MapControlLayer.h"
#include "PhysicMap.h"

std::mutex pmhmutex;

PublicMsgHandler::PublicMsgHandler()
{
	this->onEnter();
	this->onEnterTransitionDidFinish();
	this->scheduleUpdate();
	m_pMap = NULL;
}

PublicMsgHandler::~PublicMsgHandler()
{
	this->unscheduleUpdate();
	for (int i = 0; i < m_pData.size(); i++)
	{
		delete m_pData[i];
	}
	m_pData.clear();
	m_pTile.clear();
}

void PublicMsgHandler::update( float fDelta )
{
	
    if(m_pData.size()>0)
    {
		pmhmutex.lock();
        if(m_pMap != NULL)
        {
            PhysicMap* pPhysicMap = ((MapControl*)m_pMap)->getMap();
            int nCount = m_pTile.size();
            for (int i=0;i<nCount;i++)
            {   
				/*
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, i, pPhysicMap]()->void{
					
				});
				*/
				RawTile* pTile = m_pTile[i];
				Image* pData = new Image();
				pData->initWithImageData(m_pData[i]->getBytes(), m_pData[i]->getSize());
				pPhysicMap->getTileResolver()->updateMap(pTile, pData);
            }
            m_pTile.clear();
            m_pData.clear();
			
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([this]()->void{
				((MapControl*)m_pMap)->updateScreen();
			});
        }
		pmhmutex.unlock();
    }		
}

void PublicMsgHandler::addEvent(RawTile* pTile, Data* pData)
{
	pmhmutex.lock();
	m_pTile.push_back(pTile);
	m_pData.push_back(pData);
	pmhmutex.unlock();
}

void PublicMsgHandler::attach( void* pMap )
{
	m_pMap = pMap;
}
