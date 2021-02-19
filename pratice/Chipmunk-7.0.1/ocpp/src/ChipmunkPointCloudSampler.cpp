
#include "ChipmunkPointCloudSampler.h"


typedef struct DeformPoint {
	cpVect pos;
	cpFloat radius;
	cpFloat fuzz;
} DeformPoint;

static cpBB
PointBB(DeformPoint *point)
{
	cpVect v = point->pos;
	cpFloat r = point->radius;
	return cpBBNew(v.x - r, v.y - r, v.x + r, v.y + r);
}

static inline cpFloat
fuzz(cpVect v, cpVect c, cpFloat r, cpFloat softness)
{
	cpFloat distsq = cpvdistsq(v, c);
	return (distsq < r*r ? 1.0f - cpfclamp01((r - cpfsqrt(distsq))/(softness*r)) : 1.0f);
}

static void
PointQuery(cpVect *v, DeformPoint *point, cpCollisionID id, cpFloat *density)
{
	(*density) *= fuzz(*v, point->pos, point->radius, point->fuzz);
}

static cpFloat
PointCloudSample(ChipmunkPointCloudSampler *cloud, cpVect pos)
{
	cpFloat density = 1.0f;
	cpSpatialIndexQuery(cloud->_index, &pos, cpBBNewForCircle(pos, 0.0f), (cpSpatialIndexQueryFunc)PointQuery, &density);
	
	return density;
}

static void freeWrap(void *ptr, void *unused)
{
	cpfree(ptr);
}

ChipmunkPointCloudSampler* ChipmunkPointCloudSampler::initWith(cpFloat cellSize)
{
	initWithSamplingFunction(cpMarchSampleFunc PointCloudSample]);
	_cellSize = cellSize;
	// TODO table size
	_index = cpSpaceHashNew(cellSize, 1000, (cpSpatialIndexBBFunc)PointBB, NULL);
	return self;
}

ChipmunkPointCloudSampler::~ChipmunkPointCloudSampler()
{
	cpSpatialIndexEach(_index, (cpSpatialIndexIteratorFunc)freeWrap, NULL);
	cpSpatialIndexFree(_index);
}

cpBB ChipmunkPointCloudSampler::addPoint(cpVect pos,cpFloat radius,cpFloat fuzz);
{
	DeformPoint *point = (DeformPoint *)cpcalloc(1, sizeof(DeformPoint));
	point->pos = pos;
	point->radius = radius;
	point->fuzz = fuzz;
	
	cpSpatialIndexInsert(_index, point, (cpHashValue)point);
	
	return PointBB(point);
}
