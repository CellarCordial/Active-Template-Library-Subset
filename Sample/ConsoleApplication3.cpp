

#include "Sample.h"
using namespace FTS;

int main()
{
	TComPtr<ITest1> pTest1;
	FSampleDesc SampleDesc{ 1 };
	FSampleDesc* pSampleDesc = &SampleDesc;

	// 创建对象
	CreateSampleInstance(IID_ITest1, PPV_ARG(pTest1.GetAddressOf()), (void*)pSampleDesc);

	pTest1->SomeFunction1();

	// 访问其它接口
	ITest2* pTest2 = nullptr;
	pTest1->QueryInterface(IID_ITest2, (void**)&pTest2);

	ITest1* pTest1Ref = nullptr;
	pTest1->QueryInterface(IID_ITest1, (void**)&pTest1Ref);
	pTest1Ref->SomeFunction1();
	pTest2->SomeFunction2();

	// 访问聚合对象的接口
	ITest3* pTest3;
	pTest2->QueryInterface(IID_ITest3, PPV_ARG(&pTest3));

	pTest3->SomeFunction3();

	// 从聚合对象接口里访问外部对象的接口
	ITest2* pTest2Ref = nullptr;
	pTest3->QueryInterface(IID_ITest2, PPV_ARG(&pTest2Ref));
	pTest2Ref->SomeFunction2();
}
