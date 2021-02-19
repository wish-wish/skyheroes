#include "SqliteLocalStorage.h"

SqliteLocalStorage::SqliteLocalStorage()
{
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename("test.sqlitedb");
	sqlite3_open(fullPath.c_str(), &m_pConn);
}

SqliteLocalStorage::~SqliteLocalStorage()
{
	if(m_pConn)
	{
		sqlite3_close(m_pConn);
	}
}


Image* SqliteLocalStorage::getTile( RawTile* pTile )
{
//	return NULL;

	const char* getImg = "SELECT image FROM tiles WHERE x=%d AND y=%d AND z=%d";

	char strQuery[1024] = {0};
	sprintf(strQuery, getImg, pTile->x, pTile->y, pTile->z);

	sqlite3_stmt* ppStmt = NULL;

	int ret = sqlite3_prepare(m_pConn, strQuery, -1, &ppStmt, /*&tail*/0);
	ret = sqlite3_step(ppStmt);

	Image* pData = NULL;
	if (ret!=SQLITE_DONE)
	{
		pData = new Image;
		int length = sqlite3_column_bytes(ppStmt,0);
		const void *test = sqlite3_column_blob(ppStmt, 0);
		pData->initWithImageData((unsigned char*)test,length);
	}
	
	sqlite3_finalize(ppStmt);
	return pData;
}

void SqliteLocalStorage::putToCache( RawTile* pTile, Image* pData )
{
	
}
bool SqliteLocalStorage::isExist( RawTile* pTile )
{
	return false;
// 	const char* getImg = "SELECT COUNT(*) FROM tiles WHERE x=%d AND y=%d AND z=%d AND s=%d";
// 
// 	CCString* pQuery = CCString::createWithFormat(getImg,pTile->x,pTile->y,pTile->z);
// 
// 	sqlite3_stmt* ppStmt = NULL;
// 
// 	int ret = sqlite3_prepare(m_pConn, pQuery->getCString(), -1, &ppStmt, /*&tail*/0);
// 	ret = sqlite3_step(ppStmt);
// 
// 	Cell_Data* pData = NULL;
// 	if (ret!=SQLITE_DONE)
// 	{
// 		pData = new Cell_Data;
// 		pData->length = sqlite3_column_bytes(ppStmt,0);
// 		const void *test = sqlite3_column_blob(ppStmt, 0);
// 		pData->data = new char[pData->length];
// 		memcpy(pData->data,test,pData->length);
// 	}
// 
// 	sqlite3_finalize(ppStmt);
// 	return pData;
}
