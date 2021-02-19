
#include "ObjectiveChipmunk.h"
#include "ChipmunkAutoGeometry.h"


class ChipmunkCachedTile;

/// A tile cache enables an efficient means of updating a large deformable terrain.
/// General usage would be to pass a rectangle covering the viewport to ensureRect:
/// and calling markDirtyRect: each time a change is made that requires an area to be resampled.
class ChipmunkAbstractTileCache {
private:
	ChipmunkAbstractSampler *_sampler;
	ChipmunkSpace *_space;
	
	cpFloat _tileSize;
	cpFloat _samplesPerTile;
	cpVect _tileOffset;
	
	int _tileCount, _cacheSize;
	cpSpatialIndex *_tileIndex;
	ChipmunkCachedTile *_cacheHead, *_cacheTail;
	
	cpBB _ensuredBB;
	bool _ensuredDirty;
	
	bool _marchHard;
public:
	/// Should the marching be hard or soft?
	/// See cpMarchHard() and cpMarchSoft() for more information.
	bool marchHard;

	/// Offset of the tile grid origin.
	cpVect tileOffset;

	/// The sampling function to use.
	ChipmunkAbstractSampler *sampler;

	/// Create the cache from the given sampler, space to add the generated segments to,
	/// size of the tiles, and the number of samples for each tile.
	ChipmunkAbstractTileCache* initWith(ChipmunkAbstractSampler *sampler,ChipmunkSpace *space,cpFloat tileSize,int samplesPerTile,int cacheSize);

	/// Clear out all the cached tiles to force a full regen.
	void resetCache();

	/// Mark a region as needing an update.
	/// Geometry is not regenerated until ensureRect: is called.
	void markDirtyRect(cpBB bounds);

	/// Ensure that the given rect has been fully generated and contains no dirty rects.
	void ensureRect(cpBB bounds);

	/// Override this in a subclass to make custom polygon simplification behavior.
	/// Defaults to cpPolylineSimplifyCurves(polyline, 2.0f)
	cpPolyline *simplify(cpPolyline *polyline);

	/// Override this method to construct the segment shapes.
	/// By default, it creates a 0 radius segment and sets 1.0 for friction and elasticity and nothing else.
	ChipmunkSegmentShape *makeSegmentFor(ChipmunkBody *staticBody,cpVect from,cpVect to);
};

/// Generic tile cache. Configurable enough to be useful for most uses.
class ChipmunkBasicTileCache :public ChipmunkAbstractTileCache {
private:
	cpFloat _simplifyThreshold;
	
	cpFloat _segmentRadius;
	
	cpFloat _segmentFriction;
	cpFloat _segmentElasticity;
	
	cpShapeFilter _segmentFilter;
	
	cpCollisionType _segmentCollisionType;
public:
	/// Threshold value used by cpPolylineSimplifyCurves().
	cpFloat simplifyThreshold;

	/// Radius of the generated segments.
	cpFloat segmentRadius;

	/// Friction of the generated segments.
	cpFloat segmentFriction;

	/// Elasticity of the generated segments.
	cpFloat segmentElasticity;

	/// Collision filter of the generated segments.
	cpShapeFilter segmentFilter;

	/// Collision type of the generated segments.
	cpCollisionType segmentCollisionType;
};
