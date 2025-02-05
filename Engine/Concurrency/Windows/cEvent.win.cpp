// Include Files
//==============

#include "../cEvent.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Windows/Functions.h>

// Interface
//==========

eae6320::cResult eae6320::Concurrency::WaitForEvent( const cEvent& i_event, const unsigned int i_timeToWait_inMilliseconds )
{
    if ( i_event.m_handle )
    {
        const auto result = WaitForSingleObject( i_event.m_handle,
            ( i_timeToWait_inMilliseconds == Constants::dontTimeOut ) ? INFINITE : static_cast<DWORD>( i_timeToWait_inMilliseconds ) );
        switch ( result )
        {
        // The event was signaled
        case WAIT_OBJECT_0:
            return Results::success;
        // The time-out period elapsed before the event was signaled
        case WAIT_TIMEOUT:
            return Results::timeOut;
        // A Windows error prevented the wait
        case WAIT_FAILED:
            {
                const auto errorMessage = Windows::GetLastSystemError();
                EAE6320_ASSERTF( false, "Failed to wait for an event: %s", errorMessage.c_str() );
                Logging::OutputError( "Windows failed waiting for an event: %s", errorMessage.c_str() );
            }
            break;
        // An unexpected error occurred
        default:
            EAE6320_ASSERTF( false, "Failed to wait for an event" );
            Logging::OutputError( "Windows failed waiting for an event due to an unknown reason (this should never happen)" );
        }
        return Results::Failure;
    }
    else
    {
        EAE6320_ASSERTF( false, "An event can't be waited for until it has been initialized" );
        eae6320::Logging::OutputError( "An attempt was made to wait for an event that hadn't been initialized" );
        return eae6320::Results::Failure;
    }
}

eae6320::cResult eae6320::Concurrency::cEvent::Signal() const
{
    EAE6320_ASSERTF( m_handle, "An event can't be signaled until it has been initialized" );
    if ( SetEvent( m_handle ) != FALSE )
    {
        return Results::success;
    }
    const auto errorMessage = Windows::GetLastSystemError();
    EAE6320_ASSERTF( false, "Couldn't signal event: %s", errorMessage.c_str() );
    Logging::OutputError( "Windows failed to signal an event: %s", errorMessage.c_str() );
    return Results::Failure;
}

eae6320::cResult eae6320::Concurrency::cEvent::ResetToUnsignaled() const
{
    EAE6320_ASSERTF( m_handle, "An event can't be reset until it has been initialized" );
    if ( ResetEvent( m_handle ) != FALSE )
    {
        return Results::success;
    }
    const auto errorMessage = Windows::GetLastSystemError();
    EAE6320_ASSERTF( false, "Couldn't reset event to be unsignaled: %s", errorMessage.c_str() );
    Logging::OutputError( "Windows failed to reset an event to be unsignaled: %s", errorMessage.c_str() );
    return Results::Failure;
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Concurrency::cEvent::Initialize( const EventType i_type, const EventState i_initialState )
{
    constexpr SECURITY_ATTRIBUTES* const useDefaultSecurityAttributes = nullptr;
    constexpr char* const noName = nullptr;
    if ( (m_handle = CreateEvent( useDefaultSecurityAttributes, i_type == EventType::REMAIN_SIGNALED_UNTIL_RESET, i_initialState == EventState::SIGNALED, noName )) )
    {
        return Results::success;
    }
    const auto errorMessage = Windows::GetLastSystemError();
    EAE6320_ASSERTF( false, "Couldn't create event: %s", errorMessage.c_str() );
    Logging::OutputError( "Windows failed to create an event: %s", errorMessage.c_str() );
    return Results::Failure;
}

eae6320::Concurrency::cEvent::cEvent()
{

}

eae6320::cResult eae6320::Concurrency::cEvent::CleanUp()
{
    const auto result = Results::success;

    if ( m_handle )
    {
        if ( CloseHandle( m_handle ) == FALSE )
        {
            const auto errorMessage = Windows::GetLastSystemError();
            EAE6320_ASSERTF( false, "Couldn't close event handle: %s", errorMessage.c_str() );
            Logging::OutputError( "Windows failed to close an event handle: %s", errorMessage.c_str() );
        }
        m_handle = nullptr;
    }

    return result;
}
