
#define CP_ALLOW_PRIVATE_ACCESS
#include "ObjectiveChipmunk.h"
#include "chipmunk/cpHastySpace.h"


// Just in case the user doesn't have -ObjC in their linker flags.
// Annoyingly, this is the case more often than not.

// Private class used to wrap the statically allocated staticBody attached to each space.
@class _ChipmunkStaticBodySingleton :public ChipmunkBody {
	cpBody *_bodyPtr;
	ChipmunkSpace *space; // weak ref
}

typedef struct HandlerContext {
	ChipmunkSpace *space;
	id delegate;
	cpCollisionType typeA, typeB;
	SEL beginSelector;
	SEL preSolveSelector;
	SEL postSolveSelector;
	SEL separateSelector;
} HandlerContext;

//--------------------------------------------------ChipmunkSpace

ChipmunkSpace *ChipmunkSpace::spaceFromCPSpace(cpSpace *space)
{	
	ChipmunkSpace *obj = space->userData;
	cpAssertHard([obj isKindOfClass:[ChipmunkSpace class]], "'space->data' is not a pointer to a ChipmunkSpace object.");
	
	return obj;
}

ChipmunkSpace *ChipmunkSpace::initWithSpace(cpSpace * space)
{
	//_children = [[NSMutableSet alloc] init];
	//_handlers = [[NSMutableArray alloc] init];
		
	_space = space;
	cpSpaceSetUserData(_space, self);
		
	_staticBody = new ChipmunkBody(0.0f,0.0f);
	_staticBody.type = CP_BODY_TYPE_STATIC;
	cpSpaceSetStaticBody(_space, _staticBody.body);
	
	return this;
}

ChipmunkSpace *ChipmunkSpace::init{
	return this->initWithSpace:cpSpaceNew();
}

void *ChipmunkSpace::freeSpace
{
	cpSpaceFree(_space);
}

ChipmunkSpace::~ChipmunkSpace(){
	this->freeSpace;
	delete _staticBody;
	
}

cpSpace *ChipmunkSpace::space{ 
	return _space; 
}

// accessor macros
#define getter(type, lower, upper) \
type ChipmunkSpace::lower {return cpSpaceGet##upper(_space);}
#define setter(type, lower, upper) \
void ChipmunkSpace::set##upper:(type)value {cpSpaceSet##upper(_space, value);};
#define both(type, lower, upper) \
getter(type, lower, upper) \
setter(type, lower, upper)

both(int, iterations, Iterations);
both(cpVect, gravity, Gravity);
both(cpFloat, damping, Damping);
both(cpFloat, idleSpeedThreshold, IdleSpeedThreshold);
both(cpFloat, sleepTimeThreshold, SleepTimeThreshold);
both(cpFloat, collisionSlop, CollisionSlop);
both(cpFloat, collisionBias, CollisionBias);
both(cpTimestamp, collisionPersistence, CollisionPersistence);
getter(cpFloat, currentTimeStep, CurrentTimeStep);

bool ChipmunkSpace::isLocked{ 
	return cpSpaceIsLocked(_space); 
}

ChipmunkBody *ChipmunkSpace::staticBody{ 
	return _staticBody; 
}

typedef BOOL (*BeginProto)(id, SEL, cpArbiter *, ChipmunkSpace *);
static bool Begin(cpArbiter *arb, struct cpSpace *space, HandlerContext *ctx){return ((BeginProto)objc_msgSend)(ctx->delegate, ctx->beginSelector, arb, ctx->space);}

typedef BOOL (*PreSolveProto)(id, SEL, cpArbiter *, ChipmunkSpace *);
static bool PreSolve(cpArbiter *arb, struct cpSpace *space, HandlerContext *ctx){return ((PreSolveProto)objc_msgSend)(ctx->delegate, ctx->preSolveSelector, arb, ctx->space);}

typedef void (*PostSolveProto)(id, SEL, cpArbiter *, ChipmunkSpace *);
static void PostSolve(cpArbiter *arb, struct cpSpace *space, HandlerContext *ctx){((PostSolveProto)objc_msgSend)(ctx->delegate, ctx->postSolveSelector, arb, ctx->space);}

typedef void (*SeparateProto)(id, SEL, cpArbiter *, ChipmunkSpace *);
static void Separate(cpArbiter *arb, struct cpSpace *space, HandlerContext *ctx){((SeparateProto)objc_msgSend)(ctx->delegate, ctx->separateSelector, arb, ctx->space);}

// TODO handlers are never filtered.

void ChipmunkSpace::setDefaultCollisionHandler((id)delegate
	,(SEL)begin
	,(SEL)preSolve
	,(SEL)postSolve
	,(SEL)separate)
{
	cpCollisionType sentinel = (cpCollisionType)@"DEFAULT";
	
	HandlerContext context = {this, delegate, sentinel, sentinel, begin, preSolve, postSolve, separate};
	//NSData *data = [NSData dataWithBytes:&context length:sizeof(context)];
	[_handlers addObject:nullptr];
	
	cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(_space);
	if(begin) handler->beginFunc = (cpCollisionBeginFunc)Begin;
	if(preSolve) handler->preSolveFunc = (cpCollisionPreSolveFunc)PreSolve;
	if(postSolve) handler->postSolveFunc = (cpCollisionPostSolveFunc)PostSolve;
	if(separate) handler->separateFunc = (cpCollisionSeparateFunc)Separate;
	handler->userData = (void *)[data bytes];
}
	
void ChipmunkSpace::addCollisionHandler((id)delegate
	,cpCollisionType a,cpCollisionType b
	,(SEL)begin
	,(SEL)preSolve
	,(SEL)postSolve
	,(SEL)separate
{
	HandlerContext context = {this, delegate, a, b, begin, preSolve, postSolve, separate};
	//NSData *data = [NSData dataWithBytes:&context length:sizeof(context)];
	[_handlers addObject:nullptr];
	
	cpCollisionHandler *handler = cpSpaceAddCollisionHandler(_space, a, b);
	if(begin) handler->beginFunc = (cpCollisionBeginFunc)Begin;
	if(preSolve) handler->preSolveFunc = (cpCollisionPreSolveFunc)PreSolve;
	if(postSolve) handler->postSolveFunc = (cpCollisionPostSolveFunc)PostSolve;
	if(separate) handler->separateFunc = (cpCollisionSeparateFunc)Separate;
	handler->userData = (void *)[data bytes];
}

ChipmunkObject* ChipmunkSpace::add(ChipmunkObject *obj)
{
	/*
	if([obj conformsToProtocol:@protocol(ChipmunkBaseObject)]){
		[(NSObject<ChipmunkBaseObject> *)obj addToSpace:self];
	} else if([obj conformsToProtocol:@protocol(ChipmunkObject)]){
		for(NSObject<ChipmunkBaseObject> *child in [obj chipmunkObjects]) [self add:child];
	} else {
		[NSException raise:@"NSArgumentError" format:@"Attempted to add an object of type %@ to a ChipmunkSpace.", [obj class]];
	}
	*/
	_children->addObject(obj);
	return obj;
}

ChipmunkObject* ChipmunkSpace::remove(ChipmunkObject *obj)
{
	/*
	if([obj conformsToProtocol:@protocol(ChipmunkBaseObject)]){
		[(NSObject<ChipmunkBaseObject> *)obj removeFromSpace:self];
	} else if([obj conformsToProtocol:@protocol(ChipmunkObject)]){
		for(NSObject<ChipmunkBaseObject> *child in [obj chipmunkObjects]) [self remove:child];
	} else {
		[NSException raise:@"NSArgumentError" format:@"Attempted to remove an object of type %@ from a ChipmunkSpace.", [obj class]];
	}
	*/
	_children->removeObject(obj);
	return obj;
}

bool ChipmunkSpace::contains(ChipmunkObject * obj)
{
	return _children->containsObject(obj);
}

ChipmunkObject *ChipmunkSpace::smartAdd(ChipmunkObject *obj)
{
	if(cpSpaceIsLocked(_space)){
		this->addPostStepAddition(obj);
	} else {
		this->add(obj);
	}
	
	return obj;
}

ChipmunkObject*ChipmunkSpace::smartRemove(hipmunkObject *obj)
{
	if(cpSpaceIsLocked(_space)){
		this->addPostStepRemoval(obj);
	} else {
		this->remove(obj);
	}
	
	return obj;
}

struct PostStepTargetContext {
	id target;
	SEL selector;
};

static void
postStepPerform(cpSpace *unused, id key, struct PostStepTargetContext *context)
{
	[context->target performSelector:context->selector withObject:key];
	
	[context->target release];
	cpfree(context);
	[key release];
}

bool ChipmunkSpace::addPostStepCallback(void* target,(SEL)selector,void*key)
{
	if(!cpSpaceGetPostStepCallback(_space, key)){
		struct PostStepTargetContext *context = cpcalloc(1, sizeof(struct PostStepTargetContext));
		(*context) = (struct PostStepTargetContext){target, selector};
		cpSpaceAddPostStepCallback(_space, (cpPostStepFunc)postStepPerform, key, context);
		
		return true;
	} else {
		return false;
	}
}

static void
postStepPerformBlock(cpSpace *unused, id key, ChipmunkPostStepBlock block)
{
	block();
}

bool ChipmunkSpace::addPostStepBlock(ChipmunkPostStepBlock block, void* key)
{
	if(!cpSpaceGetPostStepCallback(_space, key)){
		cpSpaceAddPostStepCallback(_space, (cpPostStepFunc)postStepPerformBlock, key, block->clone());

		return true;
	} else {
		return false;
	}
}

void ChipmunkSpace::addPostStepAddition(ChipmunkObject *obj)
{
	this->addPostStepCallback(this,selector:@selector(add:),obj);
}

void ChipmunkSpace::addPostStepRemoval(ChipmunkObject *obj)
{
	this->addPostStepCallback(this,selector:@selector(remove:),obj);
}

- (NSArray *)pointQueryAll:(cpVect)point maxDistance:(cpFloat)maxDistance filter:(cpShapeFilter)filter
{
	NSMutableArray *array = [NSMutableArray array];
	cpSpacePointQuery_b(_space, point, maxDistance, filter, ^(cpShape *shape, cpVect p, cpFloat d, cpVect g){
		ChipmunkPointQueryInfo *info = [[ChipmunkPointQueryInfo alloc] initWithInfo:&(cpPointQueryInfo){shape, p, d, g}];
		[array addObject:info];
		[info release];
	});
	
	return array;
}

- (ChipmunkPointQueryInfo *)pointQueryNearest:(cpVect)point maxDistance:(cpFloat)maxDistance filter:(cpShapeFilter)filter
{
	cpPointQueryInfo info;
	cpSpacePointQueryNearest(_space, point, maxDistance, filter, &info);
	return (info.shape ? [[[ChipmunkPointQueryInfo alloc] initWithInfo:&info] autorelease] : nil);
}

typedef struct segmentQueryContext {
	cpVect start, end;
	NSMutableArray *array;
} segmentQueryContext;


ChipmunkBody * ChipmunkSpace::removeBody(ChipmunkBody *obj)
{

}

ChipmunkBody * ChipmunkSpace::addBody(ChipmunkBody *obj)
{

}

-(NSArray *)segmentQueryAllFrom:(cpVect)start to : (cpVect)end radius : (cpFloat)radius filter : (cpShapeFilter)filter
{
	NSMutableArray *array = [NSMutableArray array];
	cpSpaceSegmentQuery_b(_space, start, end, radius, filter, ^(cpShape *shape, cpVect p, cpVect n, cpFloat t){
		// TODO point
		ChipmunkSegmentQueryInfo *info = [[ChipmunkSegmentQueryInfo alloc] initWithInfo:&(cpSegmentQueryInfo){shape, p, n, t} start:start end:end];
		[array addObject:info];
		[info release];
	});
	
	return array;
}

- (ChipmunkSegmentQueryInfo *)segmentQueryFirstFrom:(cpVect)start to:(cpVect)end radius:(cpFloat)radius filter:(cpShapeFilter)filter
{
	cpSegmentQueryInfo info;
	cpSpaceSegmentQueryFirst(_space, start, end, radius, filter, &info);
	
	return (info.shape ? [[[ChipmunkSegmentQueryInfo alloc] initWithInfo:&info start:start end:end] autorelease] : nil);
}

- (NSArray *)bbQueryAll:(cpBB)bb filter:(cpShapeFilter)filter
{
	NSMutableArray *array = [NSMutableArray array];
	cpSpaceBBQuery_b(_space, bb, filter, ^(cpShape *shape){
		[array addObject:shape->userData];
	});
	
	return array;
}

//static void
//shapeQueryAll(cpShape *shape, cpContactPointSet *points, NSMutableArray *array)
//{
//	ChipmunkShapeQueryInfo *info = [[ChipmunkShapeQueryInfo alloc] initWithShape:shape->userData andPoints:points];
//	[array addObject:info];
//	[info release];
//}

- (NSArray *)shapeQueryAll:(ChipmunkShape *)shape
{
	NSMutableArray *array = [NSMutableArray array];
	cpSpaceShapeQuery_b(_space, shape.shape, ^(cpShape *shape, cpContactPointSet *points){
		ChipmunkShapeQueryInfo *info = [[ChipmunkShapeQueryInfo alloc] initWithShape:shape->userData andPoints:points];
		[array addObject:info];
		[info release];
	});
	
	return array;
}

- (BOOL)shapeTest:(ChipmunkShape *)shape
{
	return cpSpaceShapeQuery(_space, shape.shape, NULL, NULL);
}

static void PushBody(cpBody *body, NSMutableArray *arr){[arr addObject:body->userData];}
- (NSArray *)bodies
{
	NSMutableArray *arr = [NSMutableArray array];
	cpSpaceEachBody(_space, (cpSpaceBodyIteratorFunc)PushBody, arr);
	
	return arr;
}

static void PushShape(cpShape *shape, NSMutableArray *arr){[arr addObject:shape->userData];}
- (NSArray *)shapes
{
	NSMutableArray *arr = [NSMutableArray array];
	cpSpaceEachShape(_space, (cpSpaceShapeIteratorFunc)PushShape, arr);
	
	return arr;
}

static void PushConstraint(cpConstraint *constraint, NSMutableArray *arr){[arr addObject:constraint->userData];}
- (NSArray *)constraints
{
	NSMutableArray *arr = [NSMutableArray array];
	cpSpaceEachConstraint(_space, (cpSpaceConstraintIteratorFunc)PushConstraint, arr);
	
	return arr;
}


- (void)reindexStatic
{
	cpSpaceReindexStatic(_space);
}

- (void)reindexShape:(ChipmunkShape *)shape
{
	cpSpaceReindexShape(_space, shape.shape);
}

- (void)reindexShapesForBody:(ChipmunkBody *)body
{
	cpSpaceReindexShapesForBody(_space, body.body);
}

- (void)step:(cpFloat)dt
{
	cpSpaceStep(_space, dt);
}

//MARK: Extras

- (ChipmunkBody *)addBody:(ChipmunkBody *)obj {
	cpSpaceAddBody(_space, obj.body);
	[_children addObject:obj];
	return obj;
}

- (ChipmunkBody *)removeBody:(ChipmunkBody *)obj {
	cpSpaceRemoveBody(_space, obj.body);
	[_children removeObject:obj];
	return obj;
}


- (ChipmunkShape *)addShape:(ChipmunkShape *)obj {
	cpSpaceAddShape(_space, obj.shape);
	[_children addObject:obj];
	return obj;
}

- (ChipmunkShape *)removeShape:(ChipmunkShape *)obj {
	cpSpaceRemoveShape(_space, obj.shape);
	[_children removeObject:obj];
	return obj;
}

- (ChipmunkConstraint *)addConstraint:(ChipmunkConstraint *)obj {
	cpSpaceAddConstraint(_space, obj.constraint);
	[_children addObject:obj];
	return obj;
}

- (ChipmunkConstraint *)removeConstraint:(ChipmunkConstraint *)obj {
	cpSpaceRemoveConstraint(_space, obj.constraint);
	[_children removeObject:obj];
	return obj;
}

static ChipmunkSegmentShape *
boundSeg(ChipmunkBody *body, cpVect a, cpVect b, cpFloat radius, cpFloat elasticity,cpFloat friction, cpShapeFilter filter, cpCollisionType collisionType)
{
	ChipmunkSegmentShape *seg = [ChipmunkSegmentShape segmentWithBody:body from:a to:b radius:radius];
	seg.elasticity = elasticity;
	seg.friction = friction;
	seg.filter = filter;
	seg.collisionType = collisionType;
	
	return seg;
}

- (NSArray *)addBounds:(cpBB)bounds thickness:(cpFloat)radius
	elasticity:(cpFloat)elasticity friction:(cpFloat)friction
	filter:(cpShapeFilter)filter collisionType:(cpCollisionType)collisionType
{
	cpFloat l = bounds.l - radius;
	cpFloat b = bounds.b - radius;
	cpFloat r = bounds.r + radius;
	cpFloat t = bounds.t + radius;
	
	NSArray *segs = [[NSArrayChipmunkObject alloc] initWithArray:[NSArray arrayWithObjects:
		boundSeg(_staticBody, cpv(l,b), cpv(l,t), radius, elasticity, friction, filter, collisionType),
		boundSeg(_staticBody, cpv(l,t), cpv(r,t), radius, elasticity, friction, filter, collisionType),
		boundSeg(_staticBody, cpv(r,t), cpv(r,b), radius, elasticity, friction, filter, collisionType),
		boundSeg(_staticBody, cpv(r,b), cpv(l,b), radius, elasticity, friction, filter, collisionType),
		nil
	]];
	
	[self add:segs];
	return [segs autorelease];
}

@end


@implementation ChipmunkHastySpace

- (id)init {
	return [self initWithSpace:cpHastySpaceNew()];
}

-(void)freeSpace
{
	cpHastySpaceFree(_space);
}

- (void)step:(cpFloat)dt
{
	cpHastySpaceStep(_space, dt);
}

-(NSUInteger)threads
{
	return cpHastySpaceGetThreads(_space);
}

-(void)setThreads:(NSUInteger)threads
{
	cpHastySpaceSetThreads(_space, threads);
}

@end
