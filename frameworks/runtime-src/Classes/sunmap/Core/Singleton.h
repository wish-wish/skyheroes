#ifndef __COCOS2D_Singleton_H__
#define __COCOS2D_Singleton_H__

#include "cocos2d.h"

USING_NS_CC;

template<typename T>
class Singleton  
{ 
private:
	/** \brief Explicit private copy constructor. This is a forbidden operation.*/
	Singleton(const Singleton<T> &);

	/** \brief Private operator= . This is a forbidden operation. */
	Singleton& operator=(const Singleton<T> &);

protected:

	static T* msSingleton;

public:
	Singleton( void )
	{
		assert( !msSingleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		msSingleton = (T*)((int)this + offset);
#else
		msSingleton = static_cast< T* >( this );
#endif
	}
	~Singleton( void )
	{  assert( msSingleton );  msSingleton = 0;  }
	static T& getSingleton( void )
	{	assert( msSingleton );  return ( *msSingleton ); }
	static T* getSingletonPtr( void )
	{ return msSingleton; }

	static T* getInstance(){
		if (!msSingleton)
		{
			msSingleton = new T;
		}
		return msSingleton;
	}	
	inline static void destroyInstance();
};

template<typename T>
void Singleton<T>::destroyInstance()
{
	if (t)
	{
		delete t;
		t = 0;
	}
}

template <typename T>
T* Singleton<T>::msSingleton = 0;

#endif