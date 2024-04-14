#ifndef __FTS_SYS_CALL__
#define __FTS_SYS_CALL__

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#include <assert.h>
#include <new>

namespace FTS
{
	extern "C"
	{
		typedef signed char         INT8;
		typedef signed short        INT16;
		typedef signed int          INT32;
		typedef signed long long    INT64;
		typedef unsigned char       UINT8;
		typedef unsigned short      UINT16;
		typedef unsigned int        UINT32;
		typedef unsigned long long	UINT64;

		typedef signed long			LONG;
		typedef unsigned long		ULONG;
		typedef signed long long	LLONG;
		typedef unsigned long long	ULLONG;

		typedef float				FLOAT;
		typedef double				DOUBLE;

		typedef int					BOOL;

		typedef char				CHAR;
		typedef wchar_t				WCHAR;
	}

	typedef INT32					HRESULT;
}


#ifdef _MSC_VER
#define _NO_VTABLE __declspec(novtable)
#else
#define _NO_VTABLE
#endif

#ifndef TEXT
#define TEXT		L##x
#endif

#ifndef MAX_UINT8
#define MAX_UINT8   ((UINT8)~((UINT8)0))
#endif
#ifndef MAX_INT8
#define MAX_INT8    ((INT8)(MAX_UINT8 >> 1))
#endif
#ifndef MIN_INT8
#define MIN_INT8    ((INT8)~MAX_INT8)
#endif

#ifndef MAX_UINT16
#define MAX_UINT16  ((UINT16)~((UINT16)0))
#endif
#ifndef MAX_INT16
#define MAX_INT16   ((INT16)(MAX_UINT16 >> 1))
#endif
#ifndef MIN_INT16
#define MIN_INT16   ((INT16)~MAX_INT16)
#endif

#ifndef MAX_UINT32
#define MAX_UINT32  ((UINT32)~((UINT32)0))
#endif
#ifndef MAX_INT32
#define MAX_INT32   ((INT32)(MAX_UINT32 >> 1))
#endif
#ifndef MIN_INT32
#define MIN_INT32   ((INT32)~MAX_INT32)
#endif

#ifndef MAX_UINT64
#define MAX_UINT64  ((UINT64)~((UINT64)0))
#endif
#ifndef MAX_INT64
#define MAX_INT64   ((INT64)(MAX_UINT64 >> 1))
#endif
#ifndef MIN_INT64
#define MIN_INT64   ((INT64)~MAX_INT64)
#endif

#ifndef FAIL
#define FAIL(hRes)		(((HRESULT)(hRes)) < 0)
#endif

#ifndef SUCCESS
#define SUCCESS(hRes)		(((HRESULT)(hRes)) >= 0)
#endif

#ifndef STDFUNCCALLTYPE
#define STDFUNCCALLTYPE		__stdcall
#endif

#ifndef STDFUNCIMPL
#define STDFUNCIMPL			HRESULT STDFUNCCALLTYPE
#endif

#ifndef HR_OK
#define HR_OK									((HRESULT)0)
#endif

#ifndef HR_FALSE
#define HR_FALSE								((HRESULT)-1)
#endif

// No such interface
#ifndef HR_NOINTERFACE
#define HR_NOINTERFACE							((HRESULT)-2)
#endif

// Critical section failed
#ifndef HR_CRITSECFAILED
#define HR_CRITSECFAILED						((HRESULT)-3)
#endif

// Pointer is NULL
#ifndef HR_POINTER
#define HR_POINTER								((HRESULT)-4)
#endif

// Out of memory
#ifndef HR_OUTOFMEMORY
#define HR_OUTOFMEMORY							((HRESULT)-5)
#endif

// No aggregation
#ifndef HR_NOAGGREGATION
#define HR_NOAGGREGATION						((HRESULT)-5)
#endif

#ifndef FTSASSERT
#define FTSASSERT(expr)							assert(expr)
#endif

#ifndef NEW
#define NEW			new(std::nothrow)
#endif

namespace FTS
{

	inline UINT32 STDFUNCCALLTYPE LockedIncrement(UINT32* pdw) noexcept
	{
#if defined(WIN32) || defined(_WIN32)
		return ::InterlockedIncrement(pdw);
#endif
	}

	inline UINT32 STDFUNCCALLTYPE LockedDecrement(UINT32* pdw) noexcept
	{
#if defined(WIN32) || defined(_WIN32)
		return ::InterlockedDecrement(pdw);
#endif
	}


#if defined(WIN32) || defined(_WIN32)

	typedef CRITICAL_SECTION CriticalSection;

#endif

	inline void STDFUNCCALLTYPE EnterCritSection(CriticalSection* pSec)
	{
#if defined(WIN32) || defined(_WIN32)
		EnterCriticalSection(pSec);
#endif
	}

	inline BOOL STDFUNCCALLTYPE TryEnterCritSection(CriticalSection* pSec)
	{
#if defined(WIN32) || defined(_WIN32)
		return TryEnterCriticalSection(pSec);
#endif
	}

	inline void STDFUNCCALLTYPE LeaveCritSection(CriticalSection* pSec)
	{
#if defined(WIN32) || defined(_WIN32)
		LeaveCriticalSection(pSec);
#endif
	}

	inline BOOL STDFUNCCALLTYPE InitCritSection(CriticalSection* pSec)
	{
#if defined(WIN32) || defined(_WIN32)
		return InitializeCriticalSectionEx(pSec, 0, 0);
#endif
	}

	inline void STDFUNCCALLTYPE TermCritSection(CriticalSection* pSec)
	{
#if defined(WIN32) || defined(_WIN32)
		DeleteCriticalSection(pSec);
#endif
	}
}

#endif
