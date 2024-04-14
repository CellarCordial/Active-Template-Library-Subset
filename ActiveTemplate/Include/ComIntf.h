#ifndef __FTS_COM_INF__
#define __FTS_COM_INF__

#include "SysCall.h"

namespace FTS
{
	/**
	 * 若为 InterfaceID，	则在 Data1 上递增
	 * 若为 ClassID，		则在 Data2 上递增
	 * IUnknown 已占用 0x00000000-0x0000
	 */
	extern "C"
	typedef struct IID
	{
		UINT32 Data1;
		UINT16 Data2;
	} IID;

	typedef IID CLSID;
};

#ifndef CREFIID
#define CREFIID const IID&
#endif

#ifndef CREFCLSID
#define CREFCLSID const CLSID&
#endif

namespace FTS
{
	inline BOOL STDFUNCCALLTYPE IsEqualIUnknown(CREFIID criid)
	{
		return (
			((UINT32*)&criid)[0] == 0 &&
			((UINT32*)&criid)[1] == 0
		);
	}

	inline BOOL STDFUNCCALLTYPE IsEqualIIDs(CREFIID criid1, CREFIID criid2)
	{
		return (
			((UINT32*)&criid1)[0] == ((UINT32*)&criid2)[0] &&
			((UINT32*)&criid1)[1] == ((UINT32*)&criid2)[1] 
		);
	}


	extern "C" const IID IID_IUnknown;
	class IUnknown
	{
	public:
		virtual UINT32 STDFUNCCALLTYPE AddRef() = 0;
		virtual UINT32 STDFUNCCALLTYPE Release() = 0;
		virtual STDFUNCIMPL QueryInterface(CREFIID criid, void** ppv) = 0;
	};

}

#endif
