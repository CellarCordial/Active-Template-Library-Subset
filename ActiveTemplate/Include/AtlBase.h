#ifndef __FTS_ATL_BASE__
#define __FTS_ATL_BASE__

#include "ComIntf.h"
#include <cstring>
#include <cstdio>
#include <cstdint>

#ifndef PPV_ARG
#define PPV_ARG			(void**)
#endif

#ifndef PV_ARG
#define PV_ARG			(void*)
#endif

#ifndef DECLARE_INSTANCE_CREATE_FUNC
#define DECLARE_INSTANCE_CREATE_FUNC(x)	\
	inline STDFUNCIMPL Create##x##Instance(CREFIID criid, void** ppv, void* pvDesc = nullptr) \
	{\
		return x::CreateInstance(criid, ppv, pvDesc);						\
	}\
	inline STDFUNCIMPL Create##x##AggInstance(								\
		IUnknown* pOuterIUnk,												\
		CREFIID criid,														\
		void** ppv,															\
		void* pvDesc = nullptr)												\
	{\
		return x::CreateInstance(pOuterIUnk, criid, ppv, pvDesc);			\
	}
#endif

namespace FTS
{
	typedef HRESULT(STDFUNCCALLTYPE _IntfEntryFunc)(void* pv, CREFIID criid, void** ppv, UINT32 dw);

	typedef struct _IntfMapEntry
	{
		const IID* piid;
		UINT32 dwOffset;
		_IntfEntryFunc* pFunc;
	} FIntfMapEntry;
}

#ifndef _SIMPLEINTFENTRY
#define _SIMPLEINTFENTRY ((FTS::_IntfEntryFunc*)1)
#endif

#ifndef __PACKING
#define __PACKING	0x10000000
#endif

#ifndef OFFSET_OF_CLASS
#define OFFSET_OF_CLASS(Base, Derived)	((FTS::UINT64)(static_cast<Base*>((Derived*)__PACKING)) - __PACKING)
#endif

#ifndef OFFSET_OF_AGGREGATION
#define OFFSET_OF_AGGREGATION(a, p)		((FTS::UINT64)&reinterpret_cast<char const volatile&>((((a*)0)->p)))
#endif


#ifndef BEGIN_INTERFACE_MAP
#define BEGIN_INTERFACE_MAP(x)															\
	typedef x	_MapClass;																\
	IUnknown* GetControllingIUnk() noexcept												\
	{ \
		FTSASSERT(_GetEntries()[0].pFunc == _SIMPLEINTFENTRY);							\
		return (IUnknown*)((UINT64)this + _GetEntries()->dwOffset);						\
	}\
	STDFUNCIMPL _InternalQueryInterface(CREFIID criid, void** ppv) noexcept				\
	{\
		return this->InternalQueryInterface(this, _GetEntries(), criid, ppv);			\
	}\
	const static FTS::FIntfMapEntry* _GetEntries() noexcept								\
	{\
		static const FTS::FIntfMapEntry pEntries[] = {
#endif

// 一定要按继承顺序写
#ifndef INTERFACE_ENTRY
#define	INTERFACE_ENTRY(iid, x)							\
	{													\
		&iid,											\
		(UINT32)OFFSET_OF_CLASS(x, _MapClass),			\
		_SIMPLEINTFENTRY								\
	},
#endif

// 聚合要求外部对象有内部对象的指针
#ifndef INTERFACE_ENTRY_AGGREGATE
#define INTERFACE_ENTRY_AGGREGATE(iid, x)				\
	{													\
		&iid,											\
		(UINT32)OFFSET_OF_AGGREGATION(_MapClass, x),	\
		&_MapClass::_AggregateDelegate					\
	},
#endif

#ifndef INTERFACE_ENTRY_AGGREGATE_BLIND
#define INTERFACE_ENTRY_AGGREGATE_BLIND(x)				\
	{													\
		nullptr,										\
		(UINT32)OFFSET_OF_AGGREGATION(_MapClass, x),	\
		&_MapClass::_AggregateDelegate					\
	},
#endif


#ifndef END_INTERFACE_MAP
#define END_INTERFACE_MAP									\
	{ nullptr, 0, 0 }}; return &pEntries[0]; }				\
	virtual UINT32 STDFUNCCALLTYPE AddRef() noexcept = 0;	\
	virtual UINT32 STDFUNCCALLTYPE Release() noexcept = 0;	\
	virtual STDFUNCIMPL QueryInterface(CREFIID, void**) noexcept = 0; 
#endif


namespace FTS
{
	inline STDFUNCIMPL InlineInternalQueryInterface(
		void* pThis,
		const FIntfMapEntry* pEntries,
		CREFIID criid,
		void** ppv
	) noexcept
	{
		FTSASSERT(pThis != nullptr);
		FTSASSERT(pEntries != nullptr);
		FTSASSERT(pEntries->pFunc == _SIMPLEINTFENTRY);

		if (IsEqualIUnknown(criid))
		{
			IUnknown* pIUnk = (IUnknown*)((UINT64)pThis + pEntries->dwOffset);
			pIUnk->AddRef();
			*ppv = pIUnk;

			return HR_OK;
		}

		HRESULT hRes;
		while (true)
		{
			if (pEntries->pFunc == nullptr)
			{
				hRes = HR_NOINTERFACE;
				break;
			}

			BOOL bBlind = pEntries->piid == nullptr;
			if (bBlind || IsEqualIIDs(*(pEntries->piid), criid))
			{
				if (pEntries->pFunc == _SIMPLEINTFENTRY)
				{
					FTSASSERT(!bBlind);

					IUnknown* IUnk = (IUnknown*)((UINT64)pThis + pEntries->dwOffset);
					IUnk->AddRef();
					*ppv = IUnk;

					return HR_OK;
				}

				hRes = pEntries->pFunc(pThis, criid, ppv, pEntries->dwOffset);
				if (hRes == HR_OK)
				{
					return HR_OK;
				}
				else if (!bBlind && FAIL(hRes))
				{
					break;
				}

			}
			pEntries++;
		}

		ppv = nullptr;
		return hRes;
	}
}

#endif
