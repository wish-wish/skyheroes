#include "GeoUtils.h"


int GeoUtils::TILE_SIZE = 256;
const double GeoUtils::PI = 3.14159265;

/*
在google maps中的api是
map.fromLatLngToDivPixel(pointer)
map.fromDivPixelToLatLng(pointer)

百度api中提供的方
方法							返回值	描述
pixelToPoint(pixel:Pixel)	Point	像素坐标转换为经纬度坐标。
pointToPixel(point:Point)	Pixel	经纬度坐标转换为像素坐标。
*/

Vec2 GeoUtils::getLatLong(int x, int y, int zoom)
{
	double lon = -180; // x
	double lonWidth = 360; // width 360

	// double lat = -90; // y
	// double latHeight = 180; // height 180
	double lat = -1;
	double latHeight = 2;

	int tilesAtThisZoom = 1 << (17 - zoom);
	lonWidth = 360.0 / tilesAtThisZoom;
	lon = -180 + (x * lonWidth);
	latHeight = -2.0 / tilesAtThisZoom;
	lat = 1 + (y * latHeight);

	// convert lat and latHeight to degrees in a transverse mercator projection
	// note that in fact the coordinates go from about -85 to +85 not -90 to 90!
	latHeight += lat;
	latHeight = (2* atan(exp(PI * latHeight))) - PI/2;
	latHeight *= (180 / PI);

	lat = (2 * atan(exp(PI * lat))) - (PI / 2);
	lat *= (180 / PI);

	latHeight -= lat;

	if (lonWidth < 0) {
		lon = lon + lonWidth;
		lonWidth = -lonWidth;
	}

	if (latHeight < 0) {
		lat = lat + latHeight;
		latHeight = -latHeight;
	}

	Vec2 point;
	point.x = (int) lat;
	point.y = (int) lon;

	return point;
}

bool GeoUtils::isValid(RawTile tile)
{
	int tileCount = (int) pow(2.0, (int)(17 - tile.z));
	return (tile.x>-1 && tile.x<tileCount && tile.y>-1 && tile.y<tileCount);
}

Vec2 GeoUtils::toZoomedPixelCoords(double lat, double lng, int zoom)
{
	Vec2 normalised = GeoUtils::toNormalisedPixelCoords(lat, lng);
	double scale = (1 << (17 - zoom)) * TILE_SIZE;

	return Vec2((int)(normalised.x * scale), (int)(normalised.y * scale));
}

Vec2 GeoUtils::toNormalisedPixelCoords(double lat, double lng)
{
	if (lng > 180) {
		lng -= 360;
	}

	lng /= 360;
	lng += 0.5;

	lat = 0.5 - ((log(tan((PI / 4) + ((0.5 * PI * lat) / 180))) / PI) / 2.0);

	return Vec2(lng, lat);
}

Vec2 GeoUtils::getPixelOffsetInTile(double lat, double lng, int zoom)
{
	Vec2 pixelCoords = toZoomedPixelCoords(lat, lng, zoom);

	return Vec2((int)(pixelCoords.x) % TILE_SIZE, (int)(pixelCoords.y) % TILE_SIZE);
}

Vec2 GeoUtils::toTileXY(double lat, double lng, int zoom)
{
	Vec2 normalised = toNormalisedPixelCoords(lat, lng);
	int scale = 1 << (17 - zoom);

	return Vec2((int)(normalised.x * scale), (int)(normalised.y * scale));
}

std::string GeoUtils::TileXYToQuadKey( int tileX, int tileY, int levelOfDetail )
{
	std::string quadKey = "";
	for (int i = levelOfDetail; i > 0; i--) {
		char digit = '0';
		int mask = 1 << (i - 1);
		if ((tileX & mask) != 0) {
			digit++;
		}
		if ((tileY & mask) != 0) {
			digit++;
			digit++;
		}
		quadKey += digit;
	}
	return quadKey;
}

int GeoUtils::getZoomLevel( double x )
{
	if(x<1) x = 1/x;
	int counter = 1;
	while (x > 2) {
		counter++;
		x = x / 2;
	}
	if(x<1) return counter*-1;
	return counter;
}
