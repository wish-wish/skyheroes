

#include "ChipmunkAutoGeometry.h"

ChipmunkPolyline::~ChipmunkPolyline()
{
	cpPolylineFree(_line);
}

ChipmunkPolyline* ChipmunkPolyline::initWithPolyline(cpPolyline *line)
{
	_line=line;
	return this;
}

ChipmunkPolyline ChipmunkPolyline::fromPolyline(cpPolyline *line)
{
	return this->initWithPolyline(line);
}

cpFloat ChipmunkPolyline::momentForMass(cpFloat mass,cpVect offset)
{
	cpAssertHard([self isClosed], "Cannot compute the moment of a non-looped polyline.");
	return cpMomentForPoly(mass, _line->count - 1, _line->verts, offset, 0.0);
}

bool ChipmunkPolyline::isClosed()
{
	cpPolylineIsClosed(_line);
}

cpFloat ChipmunkPolyline::area()
{
	if(_area == 0.0 && [self isClosed]){
		_area = cpAreaForPoly(_line->count - 1, _line->verts, 0.0);
	}
	return _area;
}


cpVect ChipmunkPolyline::centroid()
{
	cpAssertHard([self isClosed], "Cannot compute the centroid of a non-looped polyline.");
	return cpCentroidForPoly(_line->count - 1, _line->verts);
}

	
ChipmunkPolyline *ChipmunkPolyline::simplifyCurves(cpFloat tolerance)
{
	return ChipmunkPolyline::fromPolyline(cpPolylineSimplifyCurves(_line, tolerance));
}

int ChipmunkPolyline::count()
{
	return _line->count;
}

const cpVect* ChipmunkPolyline::verts() 
{
	return _line->verts;
}

ChipmunkPolyline *ChipmunkPolyline::simplifyVertexes(cpFloat tolerance)
{
	return ChipmunkPolyline::fromPolyline(cpPolylineSimplifyVertexes(_line, tolerance));
}

ChipmunkPolyline *ChipmunkPolyline::toConvexHull()
{
	return ChipmunkPolyline::toConvexHull(0.0);
}

ChipmunkPolyline *ChipmunkPolyline::toConvexHull(cpFloat tolerance)
{
	return ChipmunkPolyline::fromPolyline(cpPolylineToConvexHull(_line, tolerance));
}

ChipmunkPolylineSet *ChipmunkPolyline::toConvexHulls_BETA(cpFloat tolerance)
{
	cpPolylineSet *set = cpPolylineConvexDecomposition_BETA(_line, tolerance);
	ChipmunkPolylineSet *value = ChipmunkPolylineSet::fromPolylineSet(set);
	cpPolylineSetFree(set, FALSE);
}

NSArray *ChipmunkPolyline::asChipmunkSegmentsWithBody(ChipmunkBody *body,cpFloat radius,cpVect offset)
{
	NSMutableArray *arr = [NSMutableArray arrayWithCapacity:_line->count];
	
	cpVect a = cpvadd(_line->verts[0], offset);
	for(int i=1; i<_line->count; i++){
		cpVect b = cpvadd(_line->verts[i], offset);
		[arr addObject:[ChipmunkSegmentShape segmentWithBody:body from:a to:b radius:radius]];
		a = b;
	}
	
	return arr;
}

ChipmunkPolyShape *ChipmunkPolyline::asChipmunkPolyShapeWithBody(ChipmunkBody *body,cpTransform transform,cpFloat radius)
{
	cpAssertHard([self isClosed], "Cannot create a poly shape for a non-closed polyline.");
	return [ChipmunkPolyShape polyWithBody:body count:_line->count - 1 verts:_line->verts transform:transform radius:radius];
}


ChipmunkPolylineSet* ChipmunkPolylineSet::initWithPolylineSet(cpPolylineSet *set)
{
	return ChipmunkPolylineSet::fromPolylineSet(set);
}

ChipmunkPolylineSet *ChipmunkPolylineSet::fromPolylineSet(cpPolylineSet *set)
{
	for(int i=0; i<set->count; i++) 
		_lines.push(ChipmunkPolyline::fromPolyline(set->lines[i]);
	return this;
}

int ChipmunkPolylineSet::count()
{
	return _lines.size();
}

ChipmunkPolyline *ChipmunkPolylineSet::lineAtIndex(int index)
{
 	return _lines[index];
}

ChipmunkAbstractSampler::ChipmunkAbstractSampler()
{

}

ChipmunkAbstractSampler* ChipmunkAbstractSampler::initWithSamplingFunction(cpMarchSampleFunc sampleFunc)
{
	_sampleFunc = sampleFunc;
	_marchThreshold = 0.5;
}

cpFloat ChipmunkAbstractSampler::sample(cpVect pos)
{
	return _sampleFunc(pos, self);
}

ChipmunkPolylineSet *ChipmunkAbstractSampler::march(cpBB bb,int xSamples,int ySamples,bool hard)
{
	cpPolylineSet set;
	cpPolylineSetInit(&set);
	
	(hard ? cpMarchHard : cpMarchSoft)(
		bb, xSamples, ySamples, _marchThreshold,
		(cpMarchSegmentFunc)cpPolylineSetCollectSegment, &set,
		_sampleFunc, this
	);
	
	ChipmunkPolylineSet *value = ChipmunkPolylineSet::fromPolylineSet(&set);
	
	cpPolylineSetDestroy(&set, FALSE);
	return value;
}

static cpFloat
SampleFromBlock(cpVect point, ChipmunkBlockSampler *self)
{
	return self->_block(point);
}

ChipmunkBlockSampler::ChipmunkBlockSampler()
{

}

ChipmunkBlockSampler* ChipmunkBlockSampler::initWithBlock(ChipmunkMarchSampleBlock block)
{
	initWithSamplingFunction((cpMarchSampleFunc)SampleFromBlock);
	_block=block;
	return this;
}

ChipmunkBlockSampler* ChipmunkBlockSampler::samplerWithBlock(ChipmunkMarchSampleBlock block)
{
	return initWithBlock(block);
}