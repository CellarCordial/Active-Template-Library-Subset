#ifndef __FTS_COM_CLI__
#define __FTS_COM_CLI__

#include "SysCall.h"

namespace FTS
{
	template <class T>
	class TComPtr
	{
	protected:

		UINT32 InternalAddRef() const noexcept
		{
			if (m_p != nullptr)
			{
				return m_p->AddRef();
			}
			return 0;
		}

		UINT32 InternalRelease()
		{
			UINT32 dwRef = 0;
			if (m_p != nullptr)
			{
				dwRef = m_p->Release();
				m_p = nullptr;
			}

			return dwRef;
		}

	public:
		typedef T	_InterfaceType;

		TComPtr() noexcept : m_p(nullptr)
		{
		}

		TComPtr(_InterfaceType* p) noexcept : m_p(p)
		{
			InternalAddRef();
		}

		TComPtr(const TComPtr& rOther) noexcept : m_p(rOther.m_p)
		{
			InternalAddRef();
		}

		TComPtr(TComPtr&& rrOther) noexcept : m_p(nullptr)
		{
			if (this != reinterpret_cast<TComPtr*>(&reinterpret_cast<UINT8&>(rrOther)))
			{
				Swap(rrOther);
			}
		}

		~TComPtr() noexcept
		{
			InternalRelease();
		}


		TComPtr& operator=(decltype(nullptr)) noexcept
		{
			InternalRelease();
			return *this;
		}

		TComPtr& operator=(_InterfaceType* pOther) noexcept
		{
			if (m_p != pOther)
			{
				TComPtr(pOther).Swap(*this);
			}
			return *this;
		}

		TComPtr& operator=(const TComPtr& rOther) noexcept
		{
			if (m_p != rOther.m_p)
			{
				TComPtr(rOther).Swap(*this);
			}
			return this;
		}

		TComPtr& operator=(TComPtr&& rrOther) noexcept
		{
			TComPtr(static_cast<TComPtr&&>(rrOther)).Swap(*this);
			return *this;
		}

		_InterfaceType* operator->() const noexcept
		{
			return m_p;
		}

		_InterfaceType* Get() const noexcept
		{
			return m_p;
		}

		_InterfaceType** GetAddressOf() noexcept
		{
			return &m_p;
		}

		_InterfaceType* const* GetAddressOf() const noexcept
		{
			return &m_p;
		}

		_InterfaceType** ReleaseAndGetAddressOf() noexcept
		{
			InternalRelease();
			return &m_p;
		}

		void Swap(TComPtr& rOther) noexcept
		{
			_InterfaceType* pTemp = m_p;
			m_p = rOther.m_p;
			rOther.m_p = pTemp;
		}

		void Swap(TComPtr&& rrOther) noexcept
		{
			_InterfaceType* pTemp = m_p;
			m_p = rrOther.m_p;
			rrOther.m_p = pTemp;
		}

		_InterfaceType* Detach() noexcept
		{
			_InterfaceType* pTemp = m_p;
			m_p = nullptr;
			return pTemp;
		}

		void Attach(_InterfaceType* p) noexcept
		{
			if (m_p != nullptr)
			{
				UINT32 dwRef = m_p->Release();
				FTSASSERT((dwRef != 0 || m_p != p) && TEXT("ComPtr can't attach a released interface pointer."));
			}
			m_p = p;
		}

		UINT32 Reset() noexcept
		{
			return InternalRelease();
		}

		HRESULT CopyTo(_InterfaceType** pp) const noexcept
		{
			HRESULT hRes = HR_POINTER;
			if (pp != nullptr)
			{
				InternalAddRef();
				*pp = m_p;
				hRes = HR_OK;
			}
			return hRes;
		}

		HRESULT CopyTo(CREFIID criid, void** ppv) const noexcept
		{
			return m_p->QueryInterface(criid, ppv);
		}

		HRESULT As(CREFIID criid, TComPtr<IUnknown>* pIUnkPtr) const noexcept
		{
			return m_p->QueryInterface(criid, reinterpret_cast<void**>(pIUnkPtr->ReleaseAndGetAddressOf()));
		}

	private:
		_InterfaceType* m_p;
	};

}



#endif