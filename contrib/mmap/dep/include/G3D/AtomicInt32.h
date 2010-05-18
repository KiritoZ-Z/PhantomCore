/**
 @file AtomicInt32.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-09-01
 @edited  2006-06-21
 */
#ifndef G3D_ATOMICINT32_H
#define G3D_ATOMICINT32_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"

#if defined(G3D_OSX)
	#include <libkern/OSAtomic.h>
#endif

namespace G3D {

/**
 An integer that may safely be used on different threads without
 external locking.

 On Win32, Linux, FreeBSD, and Mac OS X this is implemented without locks.  

 <B>BETA API</B>  This is unsupported and may change
 */
class AtomicInt32 {
private:
#   if defined(G3D_WIN32) 
#   if (_MSC_VER <= 1200 || defined(G3D_MINGW32))
        // On VC6 the type of the argument is non-volatile
#       define VCAST (long*)
#   else
#       define VCAST
#   endif
    volatile long           _value;
#   elif defined(G3D_OSX)
	int32_t					_value;
#   else
    volatile int32          _value;
#   endif


public:

    /** Initial value is undefined. */
    inline AtomicInt32() {}

    /** Atomic set */
    explicit inline AtomicInt32(const int32 x) {
        // APR does this assignment using InterlockedExchange
        // Morgan believes that volatile should be sufficient, however.
        _value = x;
    }

    /** Atomic set */
    inline AtomicInt32(const AtomicInt32& x) {
        _value = x._value;
    }

    /** Atomic set */
    inline const AtomicInt32& operator=(const int32 x) {
        _value = x;
        return *this;
    }

    /** Atomic set */
    inline void operator=(const AtomicInt32& x) {
        _value = x._value;
    }

    /** Returns the current value */
    inline int32 value() const {
        return _value;
    }

    /** Returns the old value, before the add. */
    inline int32 add(const int32 x) {
#       if defined(G3D_WIN32)

            return InterlockedExchangeAdd(VCAST &_value, x);

#       elif defined(G3D_LINUX) || defined(G3D_FREEBSD)

            int32 old;
            asm volatile ("lock; xaddl %0,%1"
                  : "=r"(old), "=m"(_value) /* outputs */
                  : "0"(x), "m"(_value)   /* inputs */
                  : "memory", "cc");
            return old;
            
#       elif defined(G3D_OSX)

			int32 old = _value;
			OSAtomicAdd32(x, &_value);
			return old;

#       endif
    }

    /** Returns old value. */
    inline int32 sub(const int32 x) {
        return add(-x);
    }

    inline void increment() {
#       if defined(G3D_WIN32)
            // Note: returns the newly incremented value
            InterlockedIncrement(VCAST &_value);
#       elif defined(G3D_LINUX) || defined(G3D_FREEBSD)
            add(1);
#       elif defined(G3D_OSX)
            // Note: returns the newly incremented value
			OSAtomicIncrement32(&_value);
#       endif
    }

    /** Returns zero if the result is zero after decrement, non-zero otherwise.*/
    inline uint32 decrement() {
#       if defined(G3D_WIN32)
            // Note: returns the newly decremented value
            return InterlockedDecrement(VCAST &_value) != 0;
#       elif defined(G3D_LINUX)  || defined(G3D_FREEBSD)
            unsigned char nz;

            asm volatile ("lock; decl %1;\n\t"
                          "setnz %%al"
                          : "=a" (nz)
                          : "m" (_value)
                          : "memory", "cc");
            return nz;
#       elif defined(G3D_OSX)
			// Note: returns the newly decremented value
            return OSAtomicDecrement32(&_value);
#       endif
    }


    /** Atomic test-and-set:  if <code>*this == comperand</code> then <code>*this := exchange</code> else do nothing.
        In both cases, returns the old value of <code>*this</code>.
    
        Performs an atomic comparison of this with the Comperand value. 
        If this is equal to the Comperand value, the Exchange value is stored in this.
        Otherwise, no operation is performed.

        Under VC6 the sign bit may be lost.

     */ 
    inline int32 compareAndSet(const int32 comperand, const int32 exchange) {
#       if defined(G3D_WIN32)
#          if defined(G3D_MINGW32)
                return (int32)InterlockedCompareExchange(VCAST &_value, exchange, comperand);
#          elif (_MSC_VER <= 1200)
                // Specification changed after VC6 from PVOID* to volatile int*
                return (int32)InterlockedCompareExchange((PVOID*)&_value, (PVOID)exchange, (PVOID)comperand);
#          else
                return InterlockedCompareExchange(VCAST &_value, exchange, comperand);
#          endif
#       elif defined(G3D_LINUX) || defined(G3D_FREEBSD)
            int32 ret;
            asm volatile ("lock; cmpxchgl %1, %2"
                          : "=a" (ret)
                          : "r" (exchange), "m" (_value), "0"(comperand)
                          : "memory", "cc");
            return ret;
#       elif defined(G3D_OSX)
			int32 old = _value;

			OSAtomicCompareAndSwap32(comperand, exchange, &_value);

			return old;
#       endif
    }

};

} // namespace

#endif
