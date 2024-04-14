#ifndef __FTS_ATL_COM__
#define __FTS_ATL_COM__

#include "AtlBase.h"

namespace FTS
{
	class FComFakeCriticalSection
	{
	public:
		HRESULT Init() noexcept
		{
			return HR_OK;
		}

		HRESULT Term() noexcept
		{
			return HR_OK;
		}

		HRESULT Lock() noexcept
		{
			return HR_OK;
		}

		HRESULT TryLock() noexcept
		{
			return HR_OK;
		}

		HRESULT Unlock() noexcept
		{
			return HR_OK;
		}
	};

	class FComCriticalSection
	{
	public:
		FComCriticalSection() { memset(&m_Sec, 0, sizeof(CriticalSection)); }
		~FComCriticalSection() {}

		HRESULT Lock() noexcept
		{
			EnterCritSection(&m_Sec);
			return HR_OK;
		}

		HRESULT Trylock() noexcept
		{
			if (!TryEnterCritSection(&m_Sec))
			{
				return HR_CRITSECFAILED;
			}
			return HR_OK;
		}

		HRESULT Unlock() noexcept
		{
			LeaveCritSection(&m_Sec);
			return HR_OK;
		}

		HRESULT Init() noexcept
		{
			if (!InitCritSection(&m_Sec))
			{
				return HR_CRITSECFAILED;
			}
			return HR_OK;
		}

		HRESULT Term() noexcept
		{
			TermCritSection(&m_Sec);
			return HR_OK;
		}

	private:
		CriticalSection m_Sec;
	};

	class FComSafeDeleteCriticalSection : public FComCriticalSection
	{
	public:
		FComSafeDeleteCriticalSection() : m_bInitialized(false) {}
		~FComSafeDeleteCriticalSection()
		{
			if (!m_bInitialized)
			{
				return;
			}
			m_bInitialized = false;
			FComCriticalSection::Term();
		}

		HRESULT Init()
		{
			FTSASSERT(!m_bInitialized);

			HRESULT hRes = FComCriticalSection::Init();
			if (SUCCESS(hRes))
			{
				m_bInitialized = true;
			}
			return hRes;
		}

		HRESULT Term() noexcept
		{
			if (!m_bInitialized)
			{
				return HR_OK;
			}
			m_bInitialized = false;
			return FComCriticalSection::Term();
		}

		HRESULT Lock() noexcept
		{
			FTSASSERT(m_bInitialized);

			return FComCriticalSection::Lock();
		}

		HRESULT Trylock() noexcept
		{
			FTSASSERT(m_bInitialized);

			return FComCriticalSection::Trylock();
		}

	private:
		BOOL m_bInitialized;
	};


	class FComSingleThreadModel
	{
	public:
		static UINT32 STDFUNCCALLTYPE Increment(UINT32* pdw) noexcept
		{
			return ++(*pdw);
		}

		static UINT32 STDFUNCCALLTYPE Decrement(UINT32* pdw) noexcept
		{
			return --(*pdw);
		}

		typedef FComFakeCriticalSection	CriticalSection;
		typedef FComFakeCriticalSection	AutoDeleteCriticalSection;
	};

	class FComMultiThreadModel
	{
	public:
		static UINT32 STDFUNCCALLTYPE Increment(UINT32* pdw) noexcept
		{
			return LockedIncrement(pdw);
		}

		static UINT32 STDFUNCCALLTYPE Decrement(UINT32* pdw) noexcept
		{
			return LockedDecrement(pdw);
		}

		typedef FComCriticalSection				CriticalSection;
		typedef FComSafeDeleteCriticalSection	AutoDeleteCriticalSection;
	};

	class FComMultiThreadModelNoCS
	{
	public:
		static UINT32 STDFUNCCALLTYPE Increment(UINT32* pdw) noexcept
		{
			return LockedIncrement(pdw);
		}

		static UINT32 STDFUNCCALLTYPE Decrement(UINT32* pdw) noexcept
		{
			return LockedDecrement(pdw);
		}

		typedef FComFakeCriticalSection	CriticalSection;
		typedef FComFakeCriticalSection	AutoDeleteCriticalSection;
	};


	class FComObjectRootBase
	{
	public:
		FComObjectRootBase() : m_dwRefCount(0) {}
		~FComObjectRootBase() {}

	public:

		void FinalRelease()
		{
		}

		HRESULT FinalConstruct()
		{
			return HR_OK;
		}

		void InternalFinalConstructAddRef()
		{
		}

		void InternalFinalConstructRelease()
		{
			FTSASSERT(m_dwRefCount == 0);
		}


		void SetDesc(void* pvDesc)
		{
			m_pvDesc = pvDesc;
		}


		static STDFUNCIMPL InternalQueryInterface(void* pThis, const FIntfMapEntry* pEntries, CREFIID criid, void** ppv)
		{
			FTSASSERT(ppv != nullptr);
			FTSASSERT(pThis != nullptr);

			FTSASSERT(pEntries->pFunc == _SIMPLEINTFENTRY);

			return InlineInternalQueryInterface(pThis, pEntries, criid, ppv);
		}


		UINT32 OuterAddRef()
		{
			return m_OuterIUnk->AddRef();
		}

		UINT32 OuterRelease()
		{
			return m_OuterIUnk->Release();
		}

		STDFUNCIMPL OuterQueryInterface(CREFIID crIID, void** ppv)
		{
			return m_OuterIUnk->QueryInterface(crIID, ppv);
		}

		static STDFUNCIMPL _AggregateDelegate(
			void* pv,
			CREFIID criid,
			void** ppv,
			UINT32 dw
		) noexcept
		{
			if (ppv == nullptr)
			{
				return HR_POINTER;
			}
			*ppv = nullptr;

			HRESULT hRes = HR_NOINTERFACE;
			IUnknown* pIUnk = *(IUnknown**)((UINT64)pv + dw);

			if (pIUnk != nullptr)
			{
				hRes = pIUnk->QueryInterface(criid, ppv);
			}
			return hRes;
		}

	protected:
		union
		{
			UINT32 m_dwRefCount;
			IUnknown* m_OuterIUnk;
		};
		void* m_pvDesc;
	};

	template <class ThreadModel>
	class TComObjectRoot;

	template <class ThreadModel>
	class TComObjectLock
	{
	public:
		typedef ThreadModel		_ThreadModel;

		TComObjectLock(TComObjectRoot<_ThreadModel>* pObject) : m_pObjectRoot(pObject)
		{
			if (m_pObjectRoot)
			{
				m_pObjectRoot->Lock();
			}
		}

		~TComObjectLock()
		{
			if (m_pObjectRoot)
			{
				m_pObjectRoot->Unlock();
			}
		}

	private:
		TComObjectRoot<_ThreadModel>* m_pObjectRoot;
	};

	template <>
	class TComObjectLock<FComSingleThreadModel>
	{
	public:
		typedef FComSingleThreadModel	_ThreadModel;

		TComObjectLock(TComObjectRoot<_ThreadModel>*)
		{
		}

		~TComObjectLock()
		{
		}
	};

	template <class ThreadModel>
	class TComObjectRoot : public FComObjectRootBase
	{
	public:
		typedef ThreadModel											_ThreadModel;
		typedef typename _ThreadModel::CriticalSection				_CritSec;
		typedef typename _ThreadModel::AutoDeleteCriticalSection	_AutoDelCritSec;
		typedef TComObjectLock<_ThreadModel>							_ObjectLock;

		~TComObjectRoot()
		{
		}

		HRESULT InitialConstruct()
		{
			return m_CritSec.Init();
		}

		UINT32 InternalAddRef()
		{
			FTSASSERT(m_dwRefCount != MAX_UINT32);

			return _ThreadModel::Increment(&m_dwRefCount);
		}

		UINT32 InternalRelease()
		{
			UINT32 dwCurrRef = _ThreadModel::Decrement(&m_dwRefCount);
			if (dwCurrRef >= MAX_UINT32 / 4)
			{
				FTSASSERT(0 && TEXT("This interface pointer has already been released."));
			}
			return dwCurrRef;
		}

		HRESULT Lock()
		{
			return m_CritSec.Lock();
		}

		HRESULT Trylock()
		{
			return m_CritSec.Trylock();
		}

		HRESULT Unlock()
		{
			return m_CritSec.Unlock();
		}


	protected:
		_AutoDelCritSec m_CritSec;
	};


	template <>
	class TComObjectRoot<FComSingleThreadModel> : public FComObjectRootBase
	{
	public:
		typedef FComSingleThreadModel					_ThreadModel;
		typedef _ThreadModel::CriticalSection			_CritSec;
		typedef _ThreadModel::AutoDeleteCriticalSection	_AutoDelCritSec;
		typedef TComObjectLock<_ThreadModel>				_ObjectLock;

		~TComObjectRoot()
		{
		}

		HRESULT InitialConstruct()
		{
			return HR_OK;
		}

		UINT32 InternalAddRef()
		{
			FTSASSERT(m_dwRefCount != MAX_UINT32);

			return _ThreadModel::Increment(&m_dwRefCount);
		}

		UINT32 InternalRelease()
		{
			UINT32 dwCurrRef = _ThreadModel::Decrement(&m_dwRefCount);
			if (dwCurrRef >= MAX_UINT32 / 4)
			{
				FTSASSERT(0 && TEXT("This interface pointer has already been released."));
			}
			return dwCurrRef;
		}

		HRESULT Lock()
		{
			return HR_OK;
		}

		HRESULT Trylock()
		{
			return HR_OK;
		}

		HRESULT Unlock()
		{
			return HR_OK;
		}

	};

	
	template <class Base>
	class TComObject : public Base
	{
	public:
		typedef Base	_BaseClass;

		TComObject(void*) {}

		~TComObject()
		{
			this->m_dwRefCount = MAX_UINT32 / 2;
			this->FinalRelease();
		}

		virtual UINT32 STDFUNCCALLTYPE AddRef() noexcept
		{
			return this->InternalAddRef();
		}

		virtual UINT32 STDFUNCCALLTYPE Release() noexcept
		{
			UINT32 dwCurrRef = this->InternalRelease();
			if (dwCurrRef == 0)
			{
				delete this;
			}
			return dwCurrRef;
		}
		
		virtual STDFUNCIMPL QueryInterface(CREFIID criid, void** ppv) noexcept
		{
			return this->_InternalQueryInterface(criid, ppv);
		}

	};

	template <class Base>
	class TComContainedObject : public Base
	{
	public:
		typedef Base		_BaseClass;

		TComContainedObject(void* pvControllingObject)
		{
			this->m_OuterIUnk = (IUnknown*)pvControllingObject;
		}

		virtual UINT32 STDFUNCCALLTYPE AddRef() noexcept
		{
			return this->OuterAddRef();
		}

		virtual UINT32 STDFUNCCALLTYPE Release() noexcept
		{
			return this->OuterRelease();
		}

		virtual STDFUNCIMPL QueryInterface(CREFIID criid, void** ppv) noexcept
		{
			return this->OuterQueryInterface(criid, ppv);
		}
	};


	template <class Contained>
	class TComAggObject : 
		public IUnknown,
		public TComObjectRoot<typename Contained::_ThreadModel>
	{
		typedef TComObjectRoot<typename Contained::_ThreadModel>	_ComObjectRoot;
	public:
		typedef Contained	_ContainedClass;

		TComAggObject(void* pv) : m_Contained(pv) {}

		~TComAggObject() noexcept
		{
			this->m_dwRefCount = MAX_UINT32 / 2;
			FinalRelease();
		}

		HRESULT FinalConstruct()
		{
			HRESULT hRes = _ComObjectRoot::FinalConstruct();
			hRes = m_Contained.FinalConstruct();
			return hRes;
		}

		void FinalRelease()
		{
			_ComObjectRoot::FinalRelease();
			m_Contained.FinalRelease();
		}

		virtual UINT32 STDFUNCCALLTYPE AddRef() noexcept
		{
			return this->InternalAddRef();
		}

		virtual UINT32 STDFUNCCALLTYPE Release() noexcept
		{
			UINT32 dwCurrRef = this->InternalRelease();
			if (dwCurrRef == 0)
			{
				delete this;
			}
			return dwCurrRef;
		}

		virtual STDFUNCIMPL QueryInterface(CREFIID criid, void** ppv) noexcept
		{
			if (ppv == nullptr)
			{
				return HR_POINTER;
			}
			*ppv = nullptr;

			if (IsEqualIUnknown(criid))
			{
				*ppv = (void*)(IUnknown*)this;
				AddRef();
			}
			else
			{
				return m_Contained._InternalQueryInterface(criid, ppv);
			}
			return HR_OK;
		}

	private:
		TComContainedObject<_ContainedClass> m_Contained;
	};


	template <class Object/*, class Malloc*/>
	class TComCreator
	{
	public:
		typedef Object		_Object;

		static STDFUNCIMPL CreateInstance(
			IUnknown* pOuterIUnk, 
			CREFIID criid, 
			void** ppv, 
			void* pvDesc = nullptr
		) noexcept
		{
			if (ppv == nullptr)
			{
				return HR_POINTER;
			}
			*ppv = nullptr;

			HRESULT hRes = HR_OUTOFMEMORY;
			_Object* p = nullptr;

			p = NEW _Object(pOuterIUnk);

			if (p != nullptr)
			{
				p->SetDesc(pvDesc);
				p->InternalFinalConstructAddRef();
				hRes = p->InitialConstruct();
				if (SUCCESS(hRes))
				{
					p->FinalConstruct();
				}
				p->InternalFinalConstructRelease();

				if (SUCCESS(hRes))
				{
					hRes = p->QueryInterface(criid, ppv);
				}
				if (FAIL(hRes))
				{
					delete p;
				}
			}
			return hRes;
		}
	};

	class FComFailCreator
	{
	public:

		static STDFUNCIMPL CreateInstance(
			IUnknown* pOuterIUnk,
			CREFIID criid,
			void** ppv,
			void* pvDesc = nullptr,
			IMalloc* pMalloc = nullptr
		) noexcept
		{
			if (ppv == nullptr)
			{
				return HR_POINTER;
			}
			return HR_NOAGGREGATION;
		}
	};

	template <class Object1, class Object2>
	class TComCreator2
	{
	public:

		static STDFUNCIMPL CreateInstance(
			IUnknown* pOuterIUnk,
			CREFIID criid,
			void** ppv,
			void* pvDesc = nullptr
		) noexcept
		{
			if (ppv == nullptr)
			{
				return HR_POINTER;
			}
			return pOuterIUnk == nullptr ?
				Object1::CreateInstance(pOuterIUnk, criid, ppv, pvDesc) :
				Object2::CreateInstance(pOuterIUnk, criid, ppv, pvDesc);
		}
	};
}

#ifndef DECLARE_PROTECT_FINAL_CONSTRUCT_RELEASE
#define DECLARE_PROTECT_FINAL_CONSTRUCT_RELEASE					\
	void InternalFinalConstructAddRef() { InternalAddRef(); }	\
	void InternalFinalConstructRelease() { InternalRelease(); } 
#endif


#ifndef DECLARE_NO_AGGREGATE
#define DECLARE_NO_AGGREGATE(x) public:	\
	typedef FTS::TComCreator2<TComCreator<TComObject<x>>, FComFailCreator> _CreatorClass;
#endif

#ifndef DECLARE_ALLOW_AGGREGATE
#define DECLARE_ALLOW_AGGREGATE(x) public:	\
	typedef FTS::TComCreator2<TComCreator<TComObject<x>>, TComCreator<TComAggObject<x>>> _CreatorClass;
#endif


namespace FTS
{
	template <class Object, CREFCLSID crclsid>
	class TComCoClass
	{
	public:
		typedef Object	_Object;

		// 默认允许，若不允许，则在自己实现的类里使用DECLARE_NO_AGGREGATE
		DECLARE_ALLOW_AGGREGATE(_Object)

		static CREFCLSID STDFUNCCALLTYPE GetCLSID() noexcept
		{
			return crclsid;
		}

		static STDFUNCIMPL CreateInstance(
			CREFIID criid,
			void** ppv,
			void* pvDesc = nullptr
		) noexcept
		{
			return _Object::_CreatorClass::CreateInstance(nullptr, criid, ppv, pvDesc);
		}

		static STDFUNCIMPL CreateInstance(
			IUnknown* pOuterIUnk,
			CREFIID criid,
			void** ppv,
			void* pvDesc = nullptr,
			IMalloc* pMalloc = nullptr
		) noexcept
		{
			return _Object::_CreatorClass::CreateInstance(pOuterIUnk, criid, ppv, pvDesc);
		}
	};
}

#endif