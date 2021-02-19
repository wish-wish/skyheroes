
class ChipmunkPointQueryInfo;
class ChipmunkSegmentQueryInfo;


/// Abstract base class for collsion shape types.
class ChipmunkShape :public ChipmunkBaseObject {
private:
	void* _userData;

public:
	virtual ~ChipmunkShape();
	/// Get the ChipmunkShape object associciated with a cpShape pointer.
	/// Undefined if the cpShape wasn't created using Objective-Chipmunk.
	static ChipmunkShape *shapeFromCPShape(cpShape *shape);

	/// Returns a pointer to the underlying cpShape C struct.
	cpShape *getShape();

	/// The ChipmunkBody that this shape is attached to.
	ChipmunkBody *body;

	// TODO doc
	cpFloat mass;
	cpFloat density;
	cpFloat moment;
	cpFloat area;
	cpVect centerOfGravity;

	/// The axis-aligned bounding box for this shape.
	cpBB bb;

	/// Sensor shapes send collision callback messages, but don't create a collision response.
	bool sensor;

	/// How bouncy this shape is.
	cpFloat elasticity;

	/// How much friction this shape has.
	cpFloat friction;

	
	///The velocity of the shape's surface.
	///This velocity is used in the collision response when calculating the friction only.
	cpVect surfaceVelocity;


	///An object reference used as a collision type identifier. This is used when defining collision handlers.
	///@attention Like most @c delegate properties this is a weak reference and does not call @c retain.
	cpCollisionType collisionType;

	///The collision filtering parameters of this shape.
	cpShapeFilter filter;

	/// Get the space the body is added to.
	ChipmunkSpace *getSpace();

	///An object that this shape is associated with. You can use this get a reference to your game object or controller object from within callbacks.
	///@attention Like most @c delegate properties this is a weak reference and does not call @c retain. This prevents reference cycles from occuring.
	void* userData;

	/// Update and cache the axis-aligned bounding box for this shape.
	cpBB cacheBB();

	ChipmunkPointQueryInfo *pointQuery(cpVect point);
	ChipmunkSegmentQueryInfo *segmentQueryFrom(cpVect start,cpVect end,cpFloat radius);
};


class ChipmunkPointQueryInfo {
private:
	cpPointQueryInfo _info;
public:
	virtual ~ChipmunkPointQueryInfo();
	ChipmunkPointQueryInfo* initWithInfo(cpPointQueryInfo *info);

	/// Returns a pointer to the underlying cpNearestPointQueryInfo C struct.
	cpPointQueryInfo *info();

	/// The ChipmunkShape found.
	ChipmunkShape *shape();

	/// The closest point on the surface of the shape to the point.
	cpVect point();

	/// The distance between the point and the surface of the shape.
	/// Negative distances mean that the point is that depth inside the shape.
	cpFloat distance();

	/// The gradient of the signed distance function.
	/// The same as info.point/info.dist, but accurate even for very small values of info.dist.
	cpVect gradient();
};


/// Holds collision information from segment queries. You should never need to create one.
class ChipmunkSegmentQueryInfo {
private:
	cpSegmentQueryInfo _info;
	cpVect _start, _end;
public:
	virtual ChipmunkSegmentQueryInfo();

	ChipmunkSegmentQueryInfo* initWith(cpSegmentQueryInfo *info,cpVect start,cpVect end);

	/// Returns a pointer to the underlying cpSegmentQueryInfo C struct.
	cpSegmentQueryInfo *info;

	/// The ChipmunkShape found.
	ChipmunkShape *shape;

	/// The percentage between the start and end points where the collision occurred.
	cpFloat t;

	/// The normal of the collision with the shape.
	cpVect normal;

	/// The point of the collision in absolute (world) coordinates.
	cpVect point;

	/// The distance from the start point where the collision occurred.
	cpFloat dist;

	/// The start point.
	cpVect start;

	/// The end point.
	cpVect end;
};


/// Holds collision information from segment queries. You should never need to create one.
class ChipmunkShapeQueryInfo {
private:
	ChipmunkShape *_shape;
	cpContactPointSet _contactPoints;
public:
	~ChipmunkShapeQueryInfo();
	ChipmunkShapeQueryInfo* initWith(ChipmunkShape *shape,cpContactPointSet *andPoints);
	ChipmunkShape *shape;
	cpContactPointSet *contactPoints;
};


/// A perfect circle shape.
class ChipmunkCircleShape :public ChipmunkShape
{
public:
	/// Create an autoreleased circle shape with the given radius and offset from the center of gravity.
	static ChipmunkCircleShape* circleWith(ChipmunkBody *body,cpFloat radius,cpVect offset);

	/// Initialize a circle shape with the given radius and offset from the center of gravity.
	ChipmunkCircleShape* initWith(ChipmunkBody *body,cpFloat radius,cpVect offset);

	/// The radius of the circle.
	cpFloat radius;

	/// The offset from the center of gravity.
	cpVect offset;
};


/// A beveled (rounded) segment shape.
class ChipmunkSegmentShape :public ChipmunkShape
{
public:
	/// Create an autoreleased segment shape with the given endpoints and radius.
	static ChipmunkSegmentShape* segmentWith(ChipmunkBody *body,cpVect from,cpVect to,cpFloat radius);

	/// Initialize a segment shape with the given endpoints and radius.
	ChipmunkSegmentShape* initWith(ChipmunkBody *body,cpVect from,cpVect to,cpFloat radius);

	/// Let Chipmunk know about the geometry of adjacent segments to avoid colliding with endcaps.
	void setPrevNeighbor(cpVect prev,cpVect next);

	/// The start of the segment shape.
	cpVect from;

	/// The end of the segment shape.
	cpVect to;

	/// The normal of the segment shape.
	cpVect normal;

	/// The beveling radius of the segment shape.
	cpFloat radius;
};

/// A convex polygon shape.
class ChipmunkPolyShape :public ChipmunkShape
{
public:
	/// Create an autoreleased polygon shape from the given vertexes after applying the transform and with the given rounding radius.
	static ChipmunkPolyShape* polyWithBody(ChipmunkBody *body,int count,const cpVect * verts,cpTransform transform,cpFloat radius);

	/// Create an autoreleased box shape centered on the center of gravity.
	static ChipmunkPolyShape* boxWithBody(ChipmunkBody *body,cpFloat width,cpFloat height,cpFloat radius);

	/// Create an autoreleased box shape with the given bounding box in body local coordinates and rounding radius.
	static ChipmunkPolyShape* boxWithBody(ChipmunkBody *body,cpBB bb,cpFloat radius);

	/// Initialize a polygon shape from the given vertexes after applying the transform and with the given rounding radius.
	ChipmunkPolyShape* initWithBody(ChipmunkBody *body,int count,const cpVect * verts,cpTransform transform,cpFloat radius);

	/// Initialize a box shape centered on the center of gravity.
	ChipmunkPolyShape* initWithBody(ChipmunkBody *body,cpFloat width,cpFloat height,cpFloat radius);

	/// Initialize a box shape with the given bounding box in body local coordinates and rounding radius.
	ChipmunkPolyShape* initWithBody(ChipmunkBody *body,cpBB bb,cpFloat radius);

	/// The number of vertexes in this polygon.
	int count;

	/// Get the rounding radius of the polygon.
	cpFloat radius;

	/// Access the vertexes of this polygon.
	cpVect getVertex(int index);
};
