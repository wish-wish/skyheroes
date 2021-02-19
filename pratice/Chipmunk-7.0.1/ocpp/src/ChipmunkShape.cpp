
#define CP_ALLOW_PRIVATE_ACCESS 1
#include "ObjectiveChipmunk.h"

//-------------------------------------------------------------ChipmunkShape

ChipmunkShape *ChipmunkShape::shapeFromCPShape(cpShape *shape)
{
	ChipmunkShape *obj = shape->userData;
	cpAssertHard([obj isKindOfClass:[ChipmunkShape class]], "'shape->data' is not a pointer to a ChipmunkShape object.");
	return obj;
}

ChipmunkShape::~ChipmunkShape{
	delete this->body;
	cpShapeDestroy(this->shape);
}


cpShape *ChipmunkShape::getShape()
	//[self doesNotRecognizeSelector:_cmd];
	return nullptr;
}

// accessor macros
#define getter(type, lower, upper) \
type ChipmunkShape::lower() {return cpShapeGet##upper(this->shape);}

#define ChipmunkShape::setter(type, lower, upper) \
void ChipmunkShape::set##upper(type value) {cpShapeSet##upper(this->shape, value);};

#define both(type, lower, upper) \
getter(type, lower, upper) \
setter(type, lower, upper)

both(ChipmunkBody*, body, Body)
both(cpFloat, mass, Mass)
both(cpFloat, ensity, Density)
getter(cpFloat, moment, Moment)
getter(cpFloat, area, Area)
getter(cpVect, centerOfGravity, CenterOfGravity)
getter(cpBB, bb, BB)
both(BOOL, sensor, Sensor)
both(cpFloat, elasticity, Elasticity)
both(cpFloat, friction, Friction)
both(cpVect, surfaceVelocity, SurfaceVelocity)
both(cpCollisionType, collisionType, CollisionType)
both(cpShapeFilter, filter, Filter)

ChipmunkSpace *ChipmunkShape::getSpace(){
	cpSpace *space = cpShapeGetSpace(this->shape);
	return (ChipmunkSpace *)(space ? cpSpaceGetUserData(space) : nullptr);
}

cpBB ChipmunkShape::cacheBB
{ 
	return cpShapeCacheBB(this->shape); 
}

ChipmunkPointQueryInfo *ChipmunkShape::pointQuery(cpVect point)
{
	cpPointQueryInfo info;
	cpShapePointQuery(this->shape, point, &info);
	return (info.shape ? new ChipmunkPointQueryInfo(info): nullptr);
}

ChipmunkSegmentQueryInfo *ChipmunkShape::segmentQueryFrom(cpVect start,cpVect end,cpFloat radius)
{
	cpSegmentQueryInfo info;
	if(cpShapeSegmentQuery(this->shape, start, end, radius, &info)){
		return new ChipmunkSegmentQueryInfo(&info,start,end);
	} else {
		return nullptr;
	}
}

// NSArray *ChipmunkShape::chipmunkObjects()
// { 
// 	return[NSArray arrayWithObject : self]; 
// }

void ChipmunkShape::addToSpace(ChipmunkSpace *space)
{
	space->addShape(this);
}

void ChipmunkShape::removeFromSpace(ChipmunkSpace *space)
{ 
	space->removeShape(this); 
}

//--------------------------------------------------------------ChipmunkPointQueryInfo

ChipmunkPointQueryInfo* ChipmunkPointQueryInfo::initWithInfo(cpPointQueryInfo *info)
{
	_info = info;
	return this;
}

cpPointQueryInfo *ChipmunkPointQueryInfo::info()
{ 
	return &_info; 
}

ChipmunkShape *ChipmunkPointQueryInfo::shape()
{ 
	return (_info.shape ? _info.shape->userData : nullptr); 
}

cpVect ChipmunkPointQueryInfo::point()
{ 
	return _info.point; 
}

cpFloat ChipmunkPointQueryInfo::distance()
{ 
	return _info.distance; 
}

cpVect ChipmunkPointQueryInfo::gradient()
{ 
	return _info.gradient; 
}

ChipmunkPointQueryInfo::~ChipmunkPointQueryInfo()
{
	delete this->shape;
}

//-------------------------------------------------------------ChipmunkSegmentQueryInfo

ChipmunkSegmentQueryInfo* ChipmunkSegmentQueryInfo::initWithInfo(cpSegmentQueryInfo *info,cpVect start,cpVect end)
{
	_info = (*info);
	_start = start;
	_end = end;

	return this;
}

cpSegmentQueryInfo *ChipmunkSegmentQueryInfo::info()
{ 
	return &_info; 
}
ChipmunkShape *ChipmunkSegmentQueryInfo::shape{ 
	return (_info.shape ? _info.shape->userData : nullptr); 
}
cpFloat ChipmunkSegmentQueryInfo::t()
{ 
	return _info.alpha; 
}
cpVect ChipmunkSegmentQueryInfo::normal()
{ 
	return _info.normal; 
}
cpVect ChipmunkSegmentQueryInfo::point{ 
	return _info.point; 
}
cpFloat ChipmunkSegmentQueryInfo::dist{ 
	return cpvdist(_start, _end)*_info.alpha; 
}
cpVect ChipmunkSegmentQueryInfo::start{ 
	return _start; 
}
cpVect ChipmunkSegmentQueryInfo::end{ 
	return _end; 
}

ChipmunkSegmentQueryInfo::~ChipmunkSegmentQueryInfo()
{
	delete this->shape;
}

//-------------------------------------------------------------ChipmunkShapeQueryInfo


cpContactPointSet *ChipmunkShapeQueryInfo::contactPoints()
{ 
	return &_contactPoints; 
}

ChipmunkShapeQueryInfo* ChipmunkShapeQueryInfo::initWithShape(ChipmunkShape *shape, cpContactPointSet *andPoints)
{
	this->shape = shape;
	this->contactPoints = *andPoints;
	return this;
}

ChipmunkShapeQueryInfo::~ChipmunkShapeQueryInfo(){
	delete this->shape;
}

//-------------------------------------------------ChipmunkCircleShape

ChipmunkCircleShape *ChipmunkCircleShape::circleWithBody(ChipmunkBody *body,cpFloat radius,cpVect offset)
{
	return this->initWithBody(body,radius,offset);
}

cpShape *ChipmunkCircleShape::shape()
{ 
	return (cpShape *)&_shape; 
}

ChipmunkCircleShape::initWithBody(ChipmunkBody *body,cpFloat radius,cpVect offset){
	this->body = body;
	cpCircleShapeInit(&_shape, body.body, radius, offset);
	this->shape->userData = this;
	return this;
}

cpFloat ChipmunkCircleShape::radius()
{ 
	return cpCircleShapeGetRadius((cpShape *)&_shape); 
}
cpVect ChipmunkCircleShape::offset()
{ 
	return cpCircleShapeGetOffset((cpShape *)&_shape); 
}


//------------------------------------ChipmunkSegmentShape

ChipmunkSegmentShape *ChipmunkSegmentShape::segmentWithBody(ChipmunkBody *body,cpVect from,cpVect to,cpFloat radius)
{
	return this->initWithBody(body,from,to,radius);
}

cpShape *ChipmunkSegmentShape::shape()
{ 
	return (cpShape *)&_shape; 
}

ChipmunkSegmentShape *ChipmunkSegmentShape::initWithBody(ChipmunkBody *body, cpVect from, cpVect to, cpFloat radius){
	this->body = body;
		cpSegmentShapeInit(&_shape, body.body, from, to, radius);
		this->shape->userData = this;
	}
	return this;
}

void ChipmunkSegmentShape::setPrevNeighbor(cpVect prev,cpVect nextNeighbor)
{
	cpSegmentShapeSetNeighbors((cpShape *)&_shape, prev, nextNeighbor);
}

cpVect ChipmunkSegmentShape::a(){ 
	return cpSegmentShapeGetA((cpShape *)&_shape); 
}
cpVect ChipmunkSegmentShape::b(){ 
	return cpSegmentShapeGetB((cpShape *)&_shape); 
}
cpVect ChipmunkSegmentShape::normal(){ 
	return cpSegmentShapeGetNormal((cpShape *)&_shape); 
}
cpFloat ChipmunkSegmentShape::radius(){ 
	return cpSegmentShapeGetRadius((cpShape *)&_shape); 
}

//-------------------------------------------------ChipmunkPolyShape

ChipmunkPolyShape* ChipmunkPolyShape::polyWithBody(ChipmunkBody *body, int count, const cpVect * verts, cpTransform transform, cpFloat radius)
{
	return this->initWithBody(body,count,verts,transform,radius);
}

ChipmunkPolyShape* ChipmunkPolyShape::boxWithBody(ChipmunkBody *body, int count, const cpVect * verts, cpTransform transform, cpFloat radius)
{
	return this->initWithBody(body, count, verts, transform, radius);
}

ChipmunkPolyShape* ChipmunkPolyShape::boxWithBody(ChipmunkBody * body,cpBB bb,cpFloat radius)
{
	return this->initBoxWithBody(body,bb,radius);
}

cpShape *ChipmunkPolyShape::shape(){ 
	return (cpShape *)&_shape; 
}

ChipmunkPolyShape* ChipmunkPolyShape::initWithBody(ChipmunkBody *body, int count, const cpVect * verts, cpTransform transform, cpFloat radius)
{
	this->body=body;
	cpPolyShapeInit(&_shape, body.body, count, verts, transform, radius);
	this->shape->userData = this;
	return this;
}

ChipmunkPolyShape* ChipmunkPolyShape::initBoxWithBody(ChipmunkBody *body, int count, const cpVect * verts, cpTransform transform, cpFloat radius)
{
	this->body = body;
	cpBoxShapeInit(&_shape, body.body, width, height, radius);
	this->shape->userData = this;
	return this;
}

ChipmunkPolyShape* ChipmunkPolyShape::initBoxWithBody(ChipmunkBody * body, cpBB bb, cpFloat radius)
{
	this->body = body
	cpBoxShapeInit2(&_shape, body.body, bb, radius);
	this->shape->userData = this;
	return this;
}

int ChipmunkPolyShape::count(){ 
	return cpPolyShapeGetCount((cpShape *)&_shape); 
}
cpFloat ChipmunkPolyShape::radius{ 
	return cpPolyShapeGetRadius((cpShape *)&_shape); 
}
cpVect ChipmunkPolyShape::getVertex(int index){ 
	return cpPolyShapeGetVert((cpShape *)&_shape, index); 
}
