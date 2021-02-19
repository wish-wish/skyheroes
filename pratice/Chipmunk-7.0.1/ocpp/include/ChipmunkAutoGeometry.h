
#include "ObjectiveChipmunk.h"

#include "chipmunk/cpMarch.h"
#include "chipmunk/cpPolyline.h"

class ChipmunkPolylineSet;

/// Wrapper for the cpPolyline type.
class ChipmunkPolyline {
private:
	cpPolyline *_line;
	cpFloat _area;
public:
	ChipmunkPolyline* initWithPolyline(cpPolyline *line);
	static ChipmunkPolyline fromPolyline(cpPolyline *line);

	/// Returns true if the first and last vertex are equal.
 	bool isClosed();

	/// Returns the signed area of the polyline calculated by cpAreaForPoly.
	/// Non-closed polylines return an area of 0.
 	cpFloat area();

	/// Centroid of the polyline calculated by cpCentroidForPoly.
	/// It is an error to call this on a non-closed polyline.
	cpVect centroid();

	/// Calculates the moment of inertia for a closed polyline with the given mass and offset.
	cpFloat momentForMass(cpFloat mass,cpVect offset);


	/// Vertex count.
	int count();

	/// Array of vertexes.
	const cpVect* verts(); 

	///Returns a copy of a polyline simplified by using the Douglas-Peucker algorithm.
	///This works very well on smooth or gently curved shapes, but not well on straight edged or angular shapes.

	ChipmunkPolyline *simplifyCurves(cpFloat tolerance);

	///Returns a copy of a polyline simplified by discarding "flat" vertexes.
	///This works well on straigt edged or angular shapes, not as well on smooth shapes.

	ChipmunkPolyline *simplifyVertexes(cpFloat tolerance);

	/// Generate a convex hull that contains a polyline. (closed or not)
	ChipmunkPolyline *toConvexHull();

	/// Generate an approximate convex hull that contains a polyline. (closed or not)
	ChipmunkPolyline *toConvexHull(cpFloat tolerance);

	/// Generate a set of convex hulls for a polyline.
	/// See the note on cpPolylineConvexDecomposition_BETA() for more information.
	ChipmunkPolylineSet *toConvexHulls_BETA(cpFloat tolerance);

	/// Create an array of segments for each segment in this polyline.
	NSArray *asChipmunkSegmentsWithBody(ChipmunkBody *body,cpFloat radius,cpVect offset);

	/// Create a ChipmunkPolyShape from this polyline. (Must be convex!)
	ChipmunkPolyShape *asChipmunkPolyShapeWithBody(ChipmunkBody *body,cpTransform transform,cpFloat radius);
};


/// Wrapper for the cpPolylineSet type.
class ChipmunkPolylineSet  {
private:
	std::vector<ChipmunkPolyline*> _lines;
public:

	ChipmunkPolylineSet* initWithPolylineSet(cpPolylineSet *set);
	static ChipmunkPolylineSet *fromPolylineSet(cpPolylineSet *set);
	int count();

	ChipmunkPolyline *lineAtIndex(int index);
};



///A sampler is an object that provides a basis function to build shapes from.
///This can be from a block of pixel data (loaded from a file, or dumped from the screen), or even a mathematical function such as Perlin noise.
class ChipmunkAbstractSampler {
protected:
	cpFloat _marchThreshold;
	cpMarchSampleFunc _sampleFunc;
public:
	/// The threshold passed to the cpMarch*() functions.
	/// The value of the contour you want to extract.
	cpFloat marchThreshold;

	/// Get the primitive cpMarchSampleFunc used by this sampler.
	cpMarchSampleFunc sampleFunc;

	/// Designated initializer.
	ChipmunkAbstractSampler* initWithSamplingFunction(cpMarchSampleFunc sampleFunc);

	/// Sample at a specific point.
	cpFloat sample(cpVect pos);

	/// March a certain area of the sampler.
	ChipmunkPolylineSet *march(cpBB bb,int xSamples,int ySamples,bool hard);
};


/// A simple sampler type that wraps a block as it's sampling function.
typedef cpFloat (^ChipmunkMarchSampleBlock)(cpVect point);

class ChipmunkBlockSampler :public ChipmunkAbstractSampler {
private:
	ChipmunkMarchSampleBlock _block;
public:
	/// Initializes the sampler using a copy of the passed block.
	ChipmunkBlockSampler* initWithBlock(ChipmunkMarchSampleBlock block);
 	static ChipmunkBlockSampler *samplerWithBlock(ChipmunkMarchSampleBlock block);
};

#include "ChipmunkImageSampler.h"
#include "ChipmunkPointCloudSampler.h"
#include "ChipmunkTileCache.h"

