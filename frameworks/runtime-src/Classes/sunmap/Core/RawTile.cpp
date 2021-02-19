#include "RawTile.h"

RawTile::RawTile()
{
	x = y = z = s = 0;
}

RawTile::~RawTile()
{
	x = y = z = s = 0;
}

RawTile::RawTile( int x, int y,int z,int s):x(x),y(y),z(z),s(s)
{

}

RawTile::RawTile( const RawTile& other )
{
	x = other.x;
	y = other.y;
	z = other.z;
	s = other.s;
}

std::string RawTile::toString()
{
	char strPath[1024] = {0};
	sprintf(strPath,"%d/%d/%d/%d/",
							this->s,this->z,this->x,this->y);
	return std::string(strPath);
}

