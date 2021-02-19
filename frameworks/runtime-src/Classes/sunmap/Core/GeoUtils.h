#ifndef __COCOS2D_GeoUtils_H__
#define __COCOS2D_GeoUtils_H__

#include "cocos2d.h"
#include "RawTile.h"

USING_NS_CC;

class GeoUtils
{
public:
	static int TILE_SIZE;

	static double const PI;

	static Vec2 getLatLong(int x, int y, int zoom);

	static bool isValid(RawTile tile);

	static Vec2 toNormalisedPixelCoords(double lat, double lng);

	static Vec2 toZoomedPixelCoords(double lat, double lng, int zoom);

	static Vec2 getPixelOffsetInTile(double lat, double lng, int zoom);

	static Vec2 toTileXY(double lat, double lng, int zoom);

	static std::string TileXYToQuadKey(int tileX, int tileY, int levelOfDetail);

	static int getZoomLevel(double x);

};

#endif