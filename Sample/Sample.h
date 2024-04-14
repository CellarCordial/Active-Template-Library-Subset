#ifndef __SAMPLE__
#define __SAMPLE__

#include "../ActiveTemplate/Include/AtlCom.h"
#include "../ActiveTemplate/Include/ComCli.h"
#include "SampIntf.h"

namespace FTS
{
	// 聚合对象
	extern "C" const CLSID CLSID_Sample1;
	class Sample1 :
		public TComObjectRoot<FComSingleThreadModel>,
		public TComCoClass<Sample1, CLSID_Sample1>,
		public ITest3
	{
	public:

		BEGIN_INTERFACE_MAP(Sample1)
			INTERFACE_ENTRY(IID_ITest3, ITest3)
		END_INTERFACE_MAP

		DECLARE_PROTECT_FINAL_CONSTRUCT_RELEASE
		virtual STDFUNCIMPL SomeFunction3();

	};

	DECLARE_INSTANCE_CREATE_FUNC(Sample1)


	// 构造参数
	typedef struct FSampleDesc
	{
		UINT32 dwData;
	} FSampleDesc;

	extern "C" const CLSID CLSID_Sample;
	class Sample :
		public TComObjectRoot<FComSingleThreadModel>,
		public TComCoClass<Sample, CLSID_Sample>,
		public ITest1,
		public ITest2

	{
	public:

		BEGIN_INTERFACE_MAP(Sample)
			INTERFACE_ENTRY(IID_ITest1, ITest1)
			INTERFACE_ENTRY(IID_ITest2, ITest2)
			INTERFACE_ENTRY_AGGREGATE(IID_ITest3, m_pSample1)
			INTERFACE_ENTRY_AGGREGATE_BLIND(m_pSample1)
		END_INTERFACE_MAP

		DECLARE_PROTECT_FINAL_CONSTRUCT_RELEASE
		STDFUNCIMPL FinalConstruct();
		STDFUNCIMPL FinalRelease();

		virtual STDFUNCIMPL SomeFunction1();
		virtual STDFUNCIMPL SomeFunction2();


	public:
		UINT32 m_dwTestData;
		IUnknown* m_pSample1;
	};

	DECLARE_INSTANCE_CREATE_FUNC(Sample)
}


#endif // __SAMPLE__