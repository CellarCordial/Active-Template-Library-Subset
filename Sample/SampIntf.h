#ifndef __SAMP_INTF__
#define __SAMP_INTF__

#include "../ActiveTemplate/Include/ComIntf.h"

namespace FTS
{
	extern "C" const IID IID_ITest1;
	class ITest1 : public IUnknown
	{
	public:

		virtual STDFUNCIMPL SomeFunction1() = 0;
	};

	extern "C" const IID IID_ITest2;
	class ITest2 : public IUnknown
	{
	public:

		virtual STDFUNCIMPL SomeFunction2() = 0;
	};

	extern "C" const IID IID_ITest3;
	class ITest3 : public IUnknown
	{
	public:

		virtual STDFUNCIMPL SomeFunction3() = 0;
	};


	
}


#endif // __SAMP_INTF__