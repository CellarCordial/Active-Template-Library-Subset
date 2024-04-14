#include "Sample.h"
#include <stdio.h>

namespace FTS
{
	extern "C" const CLSID CLSID_Sample = { 0x00000034, 0x0001 };
	extern "C" const CLSID CLSID_Sample1 = { 0x00000005, 0x0002 };

	STDFUNCIMPL Sample::FinalConstruct()
	{
		// 构造参数的获取
		FSampleDesc* pDesc = (FSampleDesc*)m_pvDesc;
		m_dwTestData = pDesc->dwData;
		m_dwTestData *= 2;

		// 聚合对象的构造
		CreateSample1AggInstance(GetControllingIUnk(), IID_IUnknown, PPV_ARG(&m_pSample1));

		return HR_OK;
	}

	STDFUNCIMPL Sample::FinalRelease()
	{
		// 聚合对象的析构
		m_pSample1->Release();
		return HR_OK;
	}

	STDFUNCIMPL Sample::SomeFunction1()
	{
		printf("SomeFunction1: %d\n", m_dwTestData);
		return HR_OK;
	}

	STDFUNCIMPL Sample::SomeFunction2()
	{
		printf("SomeFunction2: %d\n", m_dwTestData);
		return HR_OK;
	}

	STDFUNCIMPL Sample1::SomeFunction3()
	{
		printf("SomeFunction3: Sample1\n");
		return HR_OK;
	}

}