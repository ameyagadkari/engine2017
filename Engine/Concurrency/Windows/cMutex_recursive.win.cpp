// Include Files
//==============

#include "../cMutex_recursive.h"

// Interface
//==========

void eae6320::Concurrency::cMutexRecursive::Lock()
{
	EnterCriticalSection( &m_criticalSection );
}

eae6320::cResult eae6320::Concurrency::cMutexRecursive::LockIfPossible()
{
	return ( TryEnterCriticalSection( &m_criticalSection ) != FALSE ) ? Results::success : Results::Failure;
}

void eae6320::Concurrency::cMutexRecursive::Unlock()
{
	LeaveCriticalSection( &m_criticalSection );
}

// Initialization / Clean Up
//--------------------------

eae6320::Concurrency::cMutexRecursive::cMutexRecursive()
{
	InitializeCriticalSection( &m_criticalSection );
}

eae6320::Concurrency::cMutexRecursive::~cMutexRecursive()
{
	DeleteCriticalSection( &m_criticalSection );
}
