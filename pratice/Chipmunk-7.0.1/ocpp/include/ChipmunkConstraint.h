/**
	Constraints connect two ChipmunkBody objects together. Most often constraints are simple joints, but can also be things like motors, friction generators or servos.
	
	@htmlonly
	<object width="425" height="344">
		<param name="movie" value="http://www.youtube.com/v/ZgJJZTS0aMM?fs=1&amp;hl=en_US&amp;rel=0"></param>
		<param name="allowFullScreen" value="true"></param><param name="allowscriptaccess" value="always"></param>
		<embed src="http://www.youtube.com/v/ZgJJZTS0aMM?fs=1&amp;hl=en_US&amp;rel=0" type="application/x-shockwave-flash" allowscriptaccess="always" allowfullscreen="true" width="425" height="344"></embed>
	</object>
	@endhtmlonly
*/
class ChipmunkConstraint:public ChipmunkBaseObject {
private:
	void* _userData;
public:

	/// Returns a pointer to the underlying cpConstraint C struct.
	cpConstraint *constraint;

	/// The first ChipmunkBody the constraint controls.
	ChipmunkBody *bodyA;

	/// The second ChipmunkBody the constraint controls.
	ChipmunkBody *bodyB;

	/// Get the ChipmunkConstraint object associciated with a cpConstraint pointer.
	/// Undefined if the cpConstraint wasn't created using Objective-Chipmunk.
	static ChipmunkConstraint *constraintFromCPConstraint(cpConstraint *constraint);

	///Maximum force this constraint is allowed to use (defalts to infinity).
	///This allows joints to be pulled apart if too much force is applied to them.
	///It also allows you to use constraints as force or friction generators for controlling bodies.
	cpFloat maxForce;

	///The rate at which joint error is corrected.
	///Defaults to pow(1.0 - 0.1, 60.0) meaning that it will correct 10% of the error every 1/60th of a second.
	cpFloat errorBias;

	///Maximum rate (speed) that a joint can be corrected at (defaults to infinity).
	///Setting this value to a finite value allows you to control a joint like a servo motor.
	cpFloat maxBias;

	///Whether or not the connected bodies should checked for collisions.
	///Collisions are filtered before calling callbacks.
	///Defaults to TRUE.

	bool collideBodies;

	/// Get the most recent impulse applied by this constraint.
	cpFloat impulse;

	/// Get the space the body is added to.
	ChipmunkSpace *space;

	///An object that this constraint is associated with. You can use this get a reference to your game object or controller object from within callbacks.
	///@attention Like most @c delegate properties this is a weak reference and does not call @c retain. This prevents reference cycles from occuring.
	void* userData;

	/// Override this method to update a constraints parameters just before running the physics each step.
	void preSolve(ChipmunkSpace *space);

	/// Override this method to poll values from a constraint each frame after the physics runs.
	/// This can be used to implement breakable joints for instance.
	void postSolve(ChipmunkSpace *space);
};

///Pin joints hold a set distance between points on two bodies.
///Think of them as connecting a solid pin or rod between the two anchor points.
class ChipmunkPinJoint:public ChipmunkConstraint
{
public:
	///Create an autoreleased pin joint between the two bodies with the given anchor points.
	///The distance is calculated when the joint is initialized. It can be set explicitly using the property.
	static ChipmunkPinJoint *pinJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB);

	///Initialize a pin joint between the two bodies with the given anchor points.
	///The distance is calculated when the joint is initialized. It can be set explicitly using the property.
	ChipmunkPinJoint* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB);

	/// The anchor point on the first body.
	cpVect anchorA;

	/// The anchor point on the second body.
	cpVect anchorB;

	/// The distance between the two anchor points that the joint keeps.
	cpFloat dist;
};

///Slide joints hold the distance between points on two bodies between a minimum and a maximum.
///Think of them as a telescoping ChipmunkPinJoint.
class ChipmunkSlideJoint :public ChipmunkConstraint
{
public:
	///Create an autoreleased slide joint between the two bodies with the given anchor points and distance range.
	static ChipmunkSlideJoint *slideJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB,cpFloat min,cpFloat max);

	///Initialize a slide joint between the two bodies with the given anchor points and distance range.

	ChipmunkSlideJoint* initWith(ChipmunkBody *bodyA,ChipmunkBody bodyB,cpVect anchorA,cpVect anchorB,cpFloat min,cpFloat max);

	/// The anchor point on the first body.
	cpVect anchorA;

	/// The anchor point on the second body.
	cpVect anchorB;

	/// The minimum allowed distance between anchor points.
	cpFloat min;

	/// The maximum allowed distance between anchor points.
	cpFloat max;
};

///Pivot joints hold two points on two bodies together allowing them to rotate freely around the pivot.
class ChipmunkPivotJoint public: ChipmunkConstraint
private:

	///Create an autoreleased pivot joint between the two bodies with the two anchor points.
	///Make sure you have the bodies in the right place as the joint will fix itself as soon as you start simulating the space.
	static ChipmunkPivotJoint *pivotJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB）;

	///Create an autoreleased pivot joint between the two bodies by calculating the anchor points from the pivot point given in absolute coordinates.
	static ChipmunkPivotJoint *pivotJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect pivot);

	///Initialize a pivot joint between the two bodies with the two anchor points.
	///Make sure you have the bodies in the right place as the joint will fix itself as soon as you start simulating the space.
 	ChipmunkPivotJoint* initWithBody(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB);

	///Initialize a pivot joint between the two bodies by calculating the anchor points from the pivot point given in absolute coordinates.
	ChipmunkPivotJoint* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect pivot);

	///The anchor point on the first body.
	cpVect anchorA;

	/// The anchor point on the second body.
	cpVect anchorB;
};

///Groove joints hold a pivot point on one body to line along a line segment on another like a pin in a groove.
class ChipmunkGrooveJoint :public ChipmunkConstraint
{
public:
	///Create an autoreleased groove joint between the two bodies.
	///Make sure you have the bodies in the right place as the joint will snap into shape as soon as you start simulating the space.
	///@param grooveA The start of the line segment on the first body.
	///@param grooveB The end of the line segment on the first body.
	///@param anchorB The anchor point on the second body that is held to the line segment on the first.

	static ChipmunkGrooveJoint *grooveJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect grooveA cpVect grooveB,cpVect anchorB);


	///Initialize a groove joint between the two bodies.
	///Make sure you have the bodies in the right place as the joint will snap into shape as soon as you start simulating the space.
	///@param grooveA The start of the line segment on the first body.
	///@param grooveB The end of the line segment on the first body.
	///@param anchorB The anchor point on the second body that is held to the line segment on the first.
	ChipmunkGrooveJoint* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect grooveA cpVect grooveB,cpVect anchorB);

	/// The start point of the groove on the first body.
	cpVect grooveA;
	/// The end point of the groove on the first body.
	cpVect grooveB;

	/// The anchor point on the second body.
	cpVect anchorB;
};

///A spring with a damper.
///While a spring is not technically a constraint, the damper is. The spring forces are simply a convenience.
class ChipmunkDampedSpring :public ChipmunkConstraint
{
public:
	///Create an autoreleased damped spring between two bodies at the given anchor points.
	///@param restLength The length the spring wants to contract or expand to.
	///@param stiffness The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	///@param damping The amount of viscous damping to apply.
	static ChipmunkDampedSpring *dampedSpringWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB,cpFloat restLength,cpFloat stiffness,cpFloat damping);


	///Initialize a damped spring between two bodies at the given anchor points.
	///@param restLength The length the spring wants to contract or expand to.
	///@param stiffness The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	///@param damping The amount of viscous damping to apply.
	ChipmunkDampedSpring* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpVect anchorA,cpVect anchorB,cpFloat restLength,cpFloat stiffness,cpFloat damping);

	/// The anchor point on the first body.
	cpVect anchorA;

	/// The anchor point on the second body.
	cpVect anchorB;

	/// The length the spring wants to contract or expand to.
	cpFloat restLength;

	/// The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	cpFloat stiffness;

	/// The amount of viscous damping to apply.
	cpFloat damping;
};

///Like a ChipmunkDampedSpring, but operates in a rotational fashion.
class ChipmunkDampedRotarySpring :public ChipmunkConstraint
{
public:

	///Create an autoreleased damped rotary spring between the given bodies.
	///@param restAngle The angular offset in radians the spring attempts to keep between the two bodies.
	///@param stiffness The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	///@param damping The amount of viscous damping to apply.
	static ChipmunkDampedRotarySpring *dampedRotarySpringWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat restAngle,cpFloat stiffness,cpFloat damping);

	///Initialize a damped rotary spring between the given bodies.
	///@param restAngle The angular offset in radians the spring attempts to keep between the two bodies.
	///@param stiffness The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	///@param damping The amount of viscous damping to apply.
	ChipmunkDampedRotarySpring* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat restAngle,cpFloat stiffness,cpFloat damping);

	/// The angular offset the spring attempts to keep between the two bodies.
	cpFloat restAngle;

	/// The <a href="http://en.wikipedia.org/wiki/Young's_modulus">young's modulus</a> of the spring.
	cpFloat stiffness;

	/// The amount of viscous damping to apply.
	cpFloat damping;
};


///Constrains the angle between two bodies.
///This joint is often used in conjuction with a separate ChipmunkPivotJoint in order to limit the rotation around the pivot.
class ChipmunkRotaryLimitJoint :public ChipmunkConstraint
{
public:
	///Create an autoreleased rotary limit joint between the two bodies and angular range in radians.
	///Make sure you have the bodies in the right place as the joint will snap into shape as soon as you start simulating the space.
	static ChipmunkRotaryLimitJoint *rotaryLimitJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat min,cpFloat max);

	///Create an autoreleased rotary limit joint between the two bodies and angular range in radians.
	///Make sure you have the bodies in the right place as the joint will snap into shape as soon as you start simulating the space.
	ChipmunkRotaryLimitJoint* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat min,cpFloat max);

	/// The minimum angular delta of the joint in radians.
	cpFloat min;

	/// The maximum angular delta of the joint in radians.
	cpFloat max;
};


///Simple motors make two objects spin relative to each other.
///They are most often used with the ChipmunkConstraint.maxForce property set to a finite value.
class ChipmunkSimpleMotor :public ChipmunkConstraint
{
public:
	/// Create an autoreleased simple motor between the given bodies and relative rotation rate in radians per second.
	static ChipmunkSimpleMotor *simpleMotorWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat rate);

	/// Initialize a simple motor between the given bodies and relative rotation rate in radians per second.
	ChipmunkSimpleMotor* initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat rate);

	/// The relative rotation speed of the two bodies in radians per second.
	cpFloat rate;
};


///Gear joints constrain the rotational speed of one body to another.
///A ratio of 1.0 will lock the rotation of two bodies together, and negative ratios will cause them to spin in opposite directions.
///You can also use gear joints as rotary servos by setting ChipmunkConstraint.maxForce and ChipmunkConstraint.maxBias to finite values and changing the ChipmunkGearJoint.phase property.
class ChipmunkGearJoint :public ChipmunkConstraint
{
public:
	///Create an autoreleased gear joint between the given bodies.
	///@param phase The angular offset.
	///@param ratio The ratio of the rotational speeds.
	static ChipmunkGearJoint *gearJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat phase,cpFloat ratio);

	///Initialize a gear joint between the given bodies.
	///@param phase The angular offset in radians.
	///@param ratio The ratio of the rotational speeds.
	ChipmunkGearJoint * initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat phase,cpFloat ratio);

	/// The angular offset in radians.
	cpFloat phase;
	/// The ratio of the rotational speeds.
	cpFloat ratio;
};


///Ratchet joints create rotary ratches similar to a socket wrench.
@interface ChipmunkRatchetJoint : ChipmunkConstraint
{
public:
	///Create an autoreleased ratchet joint between the given bodies.
	///@param phase The angular offset of the ratchet positions in radians.
	///@param ratchet The angle in radians of each ratchet position. Negative values cause the ratchet to operate in the opposite direction.
	static ChipmunkRatchetJoint *ratchetJointWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat phase,cpFloat ratchet);

	///Initialize a ratchet joint between the given bodies.
	///@param phase The angular offset of the ratchet positions in radians.
	///@param ratchet The angle in radians of each ratchet position. Negative values cause the ratchet to operate in the opposite direction.
	ChipmunkRatchetJoint *initWith(ChipmunkBody *bodyA,ChipmunkBody *bodyB,cpFloat phase,cpFloat ratchet);

	/// The current ratchet position in radians.
	cpFloat angle;

	/// The angular offset of the ratchet positions in radians
	cpFloat phase;

	/// The angle in radians of each ratchet position. Negative values cause the ratchet to operate in the opposite direction.
	cpFloat ratchet;
};