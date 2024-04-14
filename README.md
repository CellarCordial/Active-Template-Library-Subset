## Active Template

This lib is likely to be refactored frequently.

可能经常改动。

This is a subset of MS ATL, comprising a series of templates embodying the fundamental ideas of implementing COM in ATL, along with a set of tools and containers.

这是一个MS ATL的子集，包含一些体现COM思想的模板，工具和容器。



### Example usage

```

namespace FTS
{
	// Aggregate object
	// 聚合对象
	extern "C" const CLSID CLSID_Sample1;
	class Sample1 :
		public ComObjectRoot<ComSingleThreadModel>,
		public ComCoClass<Sample1, CLSID_Sample1>,
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


	// Arguments for construction
	// 构造参数
	typedef struct FSampleDesc
	{
		UINT32 dwData;
	} FSampleDesc;

	extern "C" const CLSID CLSID_Sample;
	class Sample :
		public ComObjectRoot<ComSingleThreadModel>,
		public ComCoClass<Sample, CLSID_Sample>,
		public ITest1,
		public ITest2

	{
	public:
		// Data-driven interface conversion tables
		// 数据驱动的接口转换表
		BEGIN_INTERFACE_MAP(Sample)
			INTERFACE_ENTRY(IID_ITest1, ITest1)
			INTERFACE_ENTRY(IID_ITest2, ITest2)
			INTERFACE_ENTRY_AGGREGATE(IID_ITest3, m_pSample1)
			INTERFACE_ENTRY_AGGREGATE_BLIND(m_pSample1)
		END_INTERFACE_MAP

		// The default increase or decrease in the number of references 
		// when constructing/destructing an object
		// 构造/析构对象时默认引用数的增减
		DECLARE_PROTECT_FINAL_CONSTRUCT_RELEASE
		STDFUNCIMPL FinalConstruct();
		STDFUNCIMPL FinalRelease();

		virtual STDFUNCIMPL SomeFunction1();
		virtual STDFUNCIMPL SomeFunction2();



	public:
		UINT32 m_dwTestData;
		
		// Aggregate object pointer
		// 聚合对象的指针
		IUnknown* m_pSample1;
	};

	DECLARE_INSTANCE_CREATE_FUNC(Sample)
}

```

```
STDFUNCIMPL Sample::FinalConstruct()
	{
		// Get the arguments for construction
		// 构造参数的获取
		FSampleDesc* pDesc = (FSampleDesc*)m_pvDesc;
		m_dwTestData = pDesc->dwData;
		m_dwTestData *= 2;

		// Construction of aggregate object
		// 聚合对象的构造
		CreateSample1AggInstance(GetControllingIUnk(), IID_IUnknown, PPV_ARG(&m_pSample1));

		return HR_OK;
	}

	STDFUNCIMPL Sample::FinalRelease()
	{
		// Destruction of aggregate object
		// 聚合对象的析构
		m_pSample1->Release();
		return HR_OK;
	}
```

