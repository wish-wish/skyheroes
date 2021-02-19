
#define CP_DATA_POINTER_TYPE id
#define CP_GROUP_TYPE id
#define CP_COLLISION_TYPE_TYPE id


#ifdef CP_ALLOW_PRIVATE_ACCESS
	#undef CP_ALLOW_PRIVATE_ACCESS
	#include "chipmunk/chipmunk_private.h"
#else
	#include "chipmunk/chipmunk.h"
#endif

/// A category to have NSArray implement the ChipmunkObject protocol.
/// They make for very easy containers.
class ChipmunkObject
{
public:
	//- (id <NSFastEnumeration>)chipmunkObjects;
	virtual void chipmunkObjects(const std::string &name, std::function<bool(ChipmunkObject* child)> callback);
};

class ChipmunkSpace;

///This protocol is implemented by objects that know how to add themselves to a space.
///It's used internally as part of the ChipmunkObject protocol. You should never need to implement it yourself.
class ChipmunkBaseObject:public ChipmunkObject
{
public:
	void addToSpace(ChipmunkSpace *space);
	void removeFromSpace(ChipmunkSpace *space);
};

#include "ChipmunkBody.h"
#include "ChipmunkShape.h"
#include "ChipmunkConstraint.h"
#include "ChipmunkSpace.h"
#include "ChipmunkMultiGrab.h"
