
#include "ObjectiveChipmunk.h"
#include "ChipmunkAutoGeometry.h"


///A point cloud sampler allows you to perform deformable terrain like with a bitmap backed sampler,
///but without any bounds. It only requires memory for the points you add instead of large RAM chewing bitmap.
///However, unlike a bitmap, the deformation can only go one way. (i.e. You can add or remove terrain, but not both).
///Without any points, the sampler will return 1.0. Adding points will put "holes" in it causing it to return lower values.
class ChipmunkPointCloudSampler :public ChipmunkAbstractSampler {
private:
	cpFloat _cellSize;
	cpSpatialIndex *_index;
public:
    /// Initialize the sampler with the given cell size,
    /// which should roughly match the size of the points added to the sampler.
    ChipmunkPointCloudSampler* initWith(cpFloat cellSize);

    /// Add a point to the cloud and return the dirty rect for the point.
    cpBB addPoint(cpVect pos,cpFloat radius,cpFloat fuzz);
};