// Include Files
//==============

#include "../cbApplication.h"

#include <cstdlib>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/UserSettings/UserSettings.h>
#include <Engine/Windows/Functions.h>
#include <Engine/UserInput/UserInput.h>

// Helper Function Declarations
//=============================

namespace
{
    // Initialization / Clean Up
    //--------------------------

    eae6320::cResult CreateMainWindow(const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_windowName, const ATOM i_windowClass,
        const uint16_t i_resolutionWidth, const uint16_t i_resolutionHeight,
        eae6320::Application::cbApplication& io_application, HWND& o_window);
    eae6320::cResult CreateMainWindowClass(const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_mainWindowClassName,
        WNDPROC fOnMessageReceivedFromWindows, ATOM& o_mainWindowClass,
        const WORD* const i_iconId_large = nullptr, const WORD* const i_iconId_small = nullptr);
    eae6320::cResult FreeMainWindow(HWND& io_window);
    eae6320::cResult FreeMainWindowClass(const HINSTANCE i_thisInstanceOfTheApplication, ATOM& io_mainWindowClass);
}

// Interface
//==========

// Access
//-------

eae6320::cResult eae6320::Application::cbApplication::GetCurrentResolution(uint16_t& o_width, uint16_t& o_height) const
{
    if ((m_resolutionWidth != 0) && (m_resolutionHeight != 0))
    {
        o_width = m_resolutionWidth;
        o_height = m_resolutionHeight;
        return Results::success;
    }
    else
    {
        return Results::Failure;
    }
}

// Initialization / Clean Up
//--------------------------

eae6320::Application::cbApplication::cbApplication()
    :
    m_exitCode(EXIT_SUCCESS)
{

}

// Inherited Implementation
//===========================

// Access
//-------

eae6320::Application::cbApplication* eae6320::Application::cbApplication::GetApplicationFromWindow(const HWND i_window)
{
    const auto userData = GetWindowLongPtr(i_window, GWLP_USERDATA);
    auto* const application = reinterpret_cast<cbApplication*>(userData);
    EAE6320_ASSERT(application);
    EAE6320_ASSERT(application->m_mainWindow == i_window);
    return application;
}

// Implementation
//===============

// Run
//----

eae6320::cResult eae6320::Application::cbApplication::RenderFramesWhileWaitingForApplicationToExit(int& o_exitCode)
{
    // Enter an infinite loop that will continue until a WM_QUIT message is received from Windows
    MSG message{};
    do
    {
        // To send us a message, Windows will add it to a queue.
        // Most Windows applications should wait until a message is received and then react to it.
        // Real-time programs, though, must continually draw new images to the screen as fast as possible
        // and only pause momentarily when there is a Windows message to deal with.

        // This means that the first thing that must be done every iteration of the game loop is to "peek" at the message queue
        // and see if there are any messages from Windows that need to be handled
        bool hasWindowsSentAMessage;
        {
            constexpr HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = nullptr;
            constexpr unsigned int getAllMessageTypes = 0;
            constexpr unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
            hasWindowsSentAMessage = PeekMessage(&message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
                getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue) == TRUE;
        }
        if (!hasWindowsSentAMessage)
        {
            // Usually there will be no messages in the queue, and a new frame can be rendered
            Graphics::RenderFrame();
        }
        else
        {
            // If Windows _has_ sent a message, this iteration of the loop will handle it

            // First, the message must be "translated"
            // (Key presses are translated into character messages)
            TranslateMessage(&message);

            // Then, the message is sent on to the appropriate processing function.
            // This function is specified in the lpfnWndProc field of the WNDCLASSEX struct
            // used to register a class with Windows.
            // In the case of the main window for this application
            // it will always be OnMessageReceived().
            DispatchMessage(&message);
        }
    } while (message.message != WM_QUIT);

    // The exit code for the application is stored in the WPARAM of a WM_QUIT message
    o_exitCode = static_cast<int>(message.wParam);

    // Regardless of the exit code (whether the application itself "succeeded" or "failed")
    // this function did what it was supposed to successfully
    return Results::success;
}

eae6320::cResult eae6320::Application::cbApplication::ExitPlatformSpecific(const int i_exitCode)
{
    // Send a WM_CLOSE message,
    // which means that the application should terminate.
    // A standard Windows application can refuse a WM_CLOSE request,
    // but there is no way to tell from the return value whether that happened or not.
    // In order to be platform-independent
    // this base class application will always close in response to a WM_CLOSE request.
    // If a specific application wants to prompt the user before closing
    // it should do so in a platform-independent way before calling this Exit() function.
    m_exitCode = i_exitCode;

    EAE6320_ASSERT(m_mainWindow);
    constexpr WPARAM wParam_unused = 0;
    constexpr LPARAM lParam_unused = 0;
    // (The Windows API SendMessage() will end up calling cbApplication::OnMessageReceivedFromWindows() directly
    // and block until it returns)
    const auto windowsResult = SendMessage(m_mainWindow, WM_CLOSE, wParam_unused, lParam_unused);

    return Results::success;
}

LRESULT CALLBACK eae6320::Application::cbApplication::OnMessageReceivedFromWindows(const HWND i_window, const UINT i_message, const WPARAM i_wParam, const LPARAM i_lParam)
{
    // DispatchMessage() will send messages that the main window receives to this function.
    // There are many messages that get sent to a window,
    // but this application can ignore most of them
    // and let Windows handle them in the default way.

    // Process any messages that the application cares about
    // (any messages that are processed here should return a value
    // rather than letting the default processing function try to handle them a second time)
    switch (i_message)
    {
        // A window should close
    case WM_CLOSE:
    {
        // In a standard Windows application this message can be treated as a request
        // and it can be denied
        // (for example, the application could show the user an "Are you sure?" prompt).
        // In order to have this application behave the same for all platforms, however,
        // this request will always be honored.
        // No custom processing is done, and the message is passed on to the default message handler
        // (which will destroy the window, eventually leading to a WM_NCDESTROY message being received in this function)
    }
    break;
    // A window has been created
    case WM_CREATE:
    {
        // This message gets sent as a result of CreateWindowEx() being called
        // and is received here before that function returns

        // The LPARAM input parameter has information about the window's creation
        const auto& creationData = *reinterpret_cast<CREATESTRUCT*>(i_lParam);
        // Retrieve the application pointer that was provided to CreateWindowEx()
        auto* const application =
            static_cast<cbApplication*>(creationData.lpCreateParams);
        EAE6320_ASSERT(application);
        // Assign the new handle
        application->m_mainWindow = i_window;
        // Assign the application to the new window's user data
        {
            SetLastError(ERROR_SUCCESS);
            const auto previousUserData = SetWindowLongPtr(i_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
            // Presumably there is no previous user data on a brand new window
            if (previousUserData == NULL)
            {
                // Make sure that there was no error
                DWORD errorCode;
                const auto errorMessage = Windows::GetLastSystemError(&errorCode);
                if (errorCode != ERROR_SUCCESS)
                {
                    EAE6320_ASSERTF("Couldn't set main window user data: %s", errorMessage.c_str());
                    Logging::OutputError("Windows failed to set the main window's user data: %s", errorMessage.c_str());
                    // Returning -1 causes the window to be destroyed and a NULL window handle to be returned from CreateWindowEx()
                    application->m_mainWindow = NULL;
                    return -1;
                }
            }
        }

        // Return a value to indicate that this message was handled successfully
        // (the correct value is different for different messages types;
        // for WM_CREATE it is 0 on success)
        return 0;
    }
    // A window's nonclient area is being destroyed
// (because e.g. someone has clicked the X in upper-right corner)
    case WM_NCDESTROY:
    {
        // This is the last message a window will receive
        // (any child windows have already been destroyed)

        auto exitCode = EXIT_SUCCESS;
        {
            auto* const application = GetApplicationFromWindow(i_window);
            if (application)
            {
                // After this message has been processed the window's handle will be invalid
                application->m_mainWindow = NULL;
                // Get the exit code from the application
                exitCode = application->m_exitCode;
            }
        }

        // When the main window is destroyed
        // a WM_QUIT message should be sent
        // (if this isn't done the application would continue to run with no window)
        PostQuitMessage(exitCode);    // This sends a WM_QUIT message

        // Return a value to indicate that this message was handled successfully
        // (the correct value is different for different messages types;
        // for WM_NCDESTROY it is 0)
        return 0;
    }
    // A window has gained/lost focus
    case WM_ACTIVATE:
    {
        const auto active_status = LOWORD(i_wParam);
        UserInput::g_isWindowInFocus = active_status == WA_ACTIVE || active_status == WA_CLICKACTIVE;
        // Return a value to indicate that we processed this message
        // (the correct value is different for different messages types;
        // for WM_ACTIVATE it is 0 on success)
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        UserInput::g_mousePoints = MAKEPOINTS(i_lParam);

        if (!UserInput::g_isMouseTracked)
        {
            // Populate a struct defining the track mouse event functionality
            TRACKMOUSEEVENT trackMouseEvent{};
            {
                // Size of the track mouse event struct
                trackMouseEvent.cbSize = sizeof(trackMouseEvent);
                // The services requested
                // We are requesting one service 
                // To detect when the mouse leaves the client area of the application
                trackMouseEvent.dwFlags = TME_LEAVE;
                // A handle to the window to track
                trackMouseEvent.hwndTrack = i_window;
                // The hover time-out in milliseconds
                trackMouseEvent.dwHoverTime = 100;
            }
            // Track the mouse event
            UserInput::g_isMouseTracked = TrackMouseEvent(&trackMouseEvent);
        }
        // Return a value to indicate that we processed this message
        // (the correct value is different for different messages types;
        // for WM_MOUSEMOVE it is 0 on success)
        return 0;
    }
    case WM_MOUSELEAVE:
    {
        UserInput::g_isMouseTracked = false;
        // Return a value to indicate that we processed this message
        // (the correct value is different for different messages types;
        // for WM_MOUSELEAVE it is 0 on success)
        return 0;
    }
    default:;
    }

    // Pass any messages that weren't handled on to Windows
    // so that this application's window behaves like any standard window
    return DefWindowProc(i_window, i_message, i_wParam, i_lParam);
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Application::cbApplication::InitializeBase(const sEntryPointParameters& i_entryPointParameters)
{
    cResult result;

    // Save the handle to this specific running instance of the application
    m_thisInstanceOfTheApplication = i_entryPointParameters.applicationInstance;

    // Windows requires every window to have a "class"
    // (a window only has a single class, but many windows can share the same class)
    if (!((result = CreateMainWindowClass(m_thisInstanceOfTheApplication, GetMainWindowClassName(),
        OnMessageReceivedFromWindows, m_mainWindowClass,
        GetLargeIconId(), GetSmallIconId()))))
    {
        EAE6320_ASSERT(false);
        goto OnExit;
    }
    // Create the main window
    {
        HWND window;
        uint16_t desiredResolutionWidth, desiredResolutionHeight;
        {
            // Get the application's default
            GetDefaultInitialResolution(desiredResolutionWidth, desiredResolutionHeight);
            // Override with the user's desired resolution
            UserSettings::GetDesiredInitialResolutionWidth(desiredResolutionWidth);
            UserSettings::GetDesiredInitialResolutionHeight(desiredResolutionHeight);
        }
        if ((result = CreateMainWindow(m_thisInstanceOfTheApplication, GetMainWindowName(), m_mainWindowClass,
            desiredResolutionWidth, desiredResolutionHeight,
            *this, window)))
        {
            // The new window handle should have been assigned to the application in OnMessageReceivedFromWindows() WM_CREATE
            EAE6320_ASSERT(window == m_mainWindow);
            // Assign the resolution
            // (the window's border can't be resized, so for our class it's ok to set the resolution once and leave it)
            m_resolutionWidth = desiredResolutionWidth;
            m_resolutionHeight = desiredResolutionHeight;
        }
        else
        {
            EAE6320_ASSERT(false);
            goto OnExit;
        }
    }
    // Display the window in the initial state that Windows requested
    {
        const auto wasWindowPreviouslyVisible = ShowWindow(m_mainWindow, i_entryPointParameters.initialWindowDisplayState);
    }

OnExit:

    return result;
}

eae6320::cResult eae6320::Application::cbApplication::PopulateGraphicsInitializationParameters(Graphics::sInitializationParameters& o_initializationParameters) const
{
    EAE6320_ASSERT(m_mainWindow != NULL);
    o_initializationParameters.mainWindow = m_mainWindow;
    o_initializationParameters.resolutionWidth = m_resolutionWidth;
    o_initializationParameters.resolutionHeight = m_resolutionHeight;
#if defined( EAE6320_PLATFORM_GL )
    o_initializationParameters.thisInstanceOfTheApplication = m_thisInstanceOfTheApplication;
#endif
    return Results::success;
}

eae6320::cResult eae6320::Application::cbApplication::PopulateUserOutputInitializationParameters(UserOutput::sInitializationParameters& o_initializationParameters) const
{
    EAE6320_ASSERT(m_mainWindow != NULL);
    o_initializationParameters.mainWindow = m_mainWindow;
    return Results::success;
}

eae6320::cResult eae6320::Application::cbApplication::CleanUpBase()
{
    auto result = Results::success;

    // Main Window
    if (m_mainWindow)
    {
        const auto localResult = FreeMainWindow(m_mainWindow);
        if (!localResult)
        {
            EAE6320_ASSERT(false);
            if (result)
            {
                result = localResult;
            }
        }
    }
    // Main Window Class
    if (m_mainWindowClass)
    {
        const auto localResult = FreeMainWindowClass(m_thisInstanceOfTheApplication, m_mainWindowClass);
        if (!localResult)
        {
            EAE6320_ASSERT(false);
            if (result)
            {
                result = localResult;
            }
        }
    }

    return result;
}

// Helper Function Definitions
//============================

namespace
{
    // Initialization / Clean Up
    //--------------------------

    eae6320::cResult CreateMainWindow(const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_windowName, const ATOM i_windowClass,
        const uint16_t i_resolutionWidth, const uint16_t i_resolutionHeight,
        eae6320::Application::cbApplication& io_application, HWND& o_window)
    {
        // Create the window
        {
            // The window's style
            constexpr DWORD windowStyle =
                // "Overlapped" is basically the same as "top-level"
                WS_OVERLAPPED
                // The caption is the title bar when in windowed-mode
                | WS_CAPTION
                // The window should never change dimensions, so only a minimize box is allowed
                | WS_MINIMIZEBOX
                // The system menu appears when you right-click the title bar
                | WS_SYSMENU;
            // The window's extended style
            constexpr DWORD windowStyle_extended =
                // The following is a macro to make the extended style the default top-level look
                WS_EX_OVERLAPPEDWINDOW;
            // The width and height of the window.
            // A game cares about the width and height of the actual "client area",
            // which is the part of the window that doesn't include the borders and title bar;
            // this means that if we say that a game runs at a resolution of 800 x 600,
            // the actual window will be slightly bigger than that.
            // Initially, then, the window will be created with default values that Windows chooses
            // and then resized after creation)
            constexpr int width = CW_USEDEFAULT;
            constexpr int height = CW_USEDEFAULT;
            // The initial position of the window
            // (We don't care, and will let Windows decide)
            constexpr int position_x = CW_USEDEFAULT;
            constexpr int position_y = CW_USEDEFAULT;
            // Handle to the parent of this window
            // (Since this is our main window, it can't have a parent)
            constexpr HWND parent = NULL;
            // Handle to the menu for this window
            // (The main window won't have a menu)
            constexpr HMENU menu = NULL;
            // A pointer can be sent with the WM_CREATE message received in OnMessageReceivedFromWindows()
            void* const userData = &io_application;

            // Ask Windows to create the specified window.
            // CreateWindowEx() will return a "handle" to the window (kind of like a pointer),
            // which is what we'll use when communicating with Windows to refer to this window
            o_window = CreateWindowEx(windowStyle_extended, MAKEINTATOM(i_windowClass), i_windowName, windowStyle,
                position_x, position_y, width, height,
                parent, menu, i_thisInstanceOfTheApplication, userData);
            if (o_window != nullptr)
            {
                eae6320::Logging::OutputMessage("Created main window \"%s\"", i_windowName);
            }
            else
            {
                const auto errorMessage = eae6320::Windows::GetLastSystemError();
                EAE6320_ASSERTF(false, "Main window wasn't created: %s", errorMessage.c_str());
                eae6320::Logging::OutputError("Windows failed to create the main window: %s", errorMessage.c_str());
                return eae6320::Results::Failure;
            }
        }

        // Change the window's size based on the desired client area resolution
        {
            // Calculate how much of the window is coming from the "non-client area"
            // (the borders and title bar)
            RECT windowCoordinates;
            struct
            {
                long width;
                long height;
            } nonClientAreaSize;
            // Calculate the coordinates to place window about the screen center
            RECT desktopCoordinates;
            struct
            {
                long x;
                long y;
            } screenCenter;
            {
                // Get the coordinates of the entire window
                if (GetWindowRect(o_window, &windowCoordinates) == FALSE)
                {
                    const auto errorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, "Couldn't get coordinates of the main window: %s", errorMessage.c_str());
                    eae6320::Logging::OutputError("Windows failed to get the coordinates of the main window: %s", errorMessage.c_str());
                    goto OnError;
                }
                // Get the dimensions of the client area
                RECT clientDimensions;
                if (GetClientRect(o_window, &clientDimensions) == FALSE)
                {
                    const auto errorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, "Couldn't get the main window's client dimensions: %s", errorMessage.c_str());
                    eae6320::Logging::OutputError("Windows failed to get the dimensions of the main window's client area:: %s",
                        errorMessage.c_str());
                    goto OnError;
                }
                // Get the difference between them
                nonClientAreaSize.width = (windowCoordinates.right - windowCoordinates.left) - clientDimensions.right;
                nonClientAreaSize.height = (windowCoordinates.bottom - windowCoordinates.top) - clientDimensions.bottom;
            }
            // Get the coordinates of the entire desktop
            {
                // Get a handle to the desktop window
                const HWND hDesktop = GetDesktopWindow();
                // Get the size of screen to the variable desktop
                if(GetWindowRect(hDesktop, &desktopCoordinates) == FALSE)
                {
                    const auto errorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, "Couldn't get coordinates of the desktop window: %s", errorMessage.c_str());
                    eae6320::Logging::OutputError("Windows failed to get the coordinates of the desktop window: %s", errorMessage.c_str());
                    goto OnError;
                }
                // Calculate the screen centre
                screenCenter.x = desktopCoordinates.right / 2;
                screenCenter.y = desktopCoordinates.bottom / 2;
            }
            // Resize the window
            {
                const int desiredWidth_window = i_resolutionWidth + nonClientAreaSize.width;
                const int desiredHeight_window = i_resolutionHeight + nonClientAreaSize.height;
                const int desiredPositionX_window = screenCenter.x - (desiredWidth_window / 2);
                const int desiredPositionY_window = screenCenter.y - (desiredHeight_window / 2);
                const BOOL redrawTheWindowAfterItsBeenResized = TRUE;
                if (MoveWindow(o_window,
                    desiredPositionX_window, desiredPositionY_window, desiredWidth_window, desiredHeight_window,
                    redrawTheWindowAfterItsBeenResized) != FALSE)
                {
                    eae6320::Logging::OutputMessage("Set main window resolution to %u x %u",
                        i_resolutionWidth, i_resolutionHeight);
                }
                else
                {
                    const auto errorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, "Couldn't resize the main window to &i x &i: %s",
                        desiredWidth_window, desiredHeight_window, errorMessage.c_str());
                    eae6320::Logging::OutputError("Windows failed to resize main window to &i x &i"
                        " (based on a desired resolution of %u x %u): %s",
                        desiredWidth_window, desiredHeight_window, i_resolutionWidth, i_resolutionHeight, errorMessage.c_str());
                    goto OnError;
                }
            }
        }

        return eae6320::Results::success;

    OnError:

        if (o_window != nullptr)
        {
            FreeMainWindow(o_window);
        }

        return eae6320::Results::Failure;
    }

    eae6320::cResult CreateMainWindowClass(const HINSTANCE i_thisInstanceOfTheApplication, const char* const i_mainWindowClassName,
        const WNDPROC fOnMessageReceivedFromWindows, ATOM& o_mainWindowClass,
        const WORD* const i_iconId_large, const WORD* const i_iconId_small)
    {
        // Populate a struct defining the window class that we want
        WNDCLASSEX wndClassEx{};
        {
            wndClassEx.cbSize = sizeof(WNDCLASSEX);
            wndClassEx.hInstance = i_thisInstanceOfTheApplication;

            // The class's style
            // (We don't have to worry about any of these)
            wndClassEx.style = 0;
            // The function that will process all of the messages
            // that Windows will send to windows of this class
            wndClassEx.lpfnWndProc = fOnMessageReceivedFromWindows;
            // Extra bytes can be set aside in the class for user data
            wndClassEx.cbClsExtra = 0;
            // Extra bytes can be set aside for each window of this class,
            // but this is usually specified for each window individually
            wndClassEx.cbWndExtra = 0;
            // The large and small icons that windows of this class should use
            if (i_iconId_large)
            {
                wndClassEx.hIcon = LoadIcon(i_thisInstanceOfTheApplication, MAKEINTRESOURCE(*i_iconId_large));
            }
            else
            {
                wndClassEx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            }
            if (i_iconId_small != nullptr)
            {
                wndClassEx.hIconSm = LoadIcon(i_thisInstanceOfTheApplication, MAKEINTRESOURCE(*i_iconId_small));
            }
            else
            {
                wndClassEx.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
            }
            // The cursor that should display when the mouse pointer is over windows of this class
            // (in a real game you would likely hide this and render your own)
            wndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
            // The "brush" that windows of this class should use as a background
            // (Setting this is a bit confusing but not important,
            // so don't be alarmed if the next line looks scary,
            // especially since 1) our entire window will be filled by the rendering and we won't see background
            // and 2) the technique below doesn't seem to work anymore since Windows Vista)
            wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>(IntToPtr(COLOR_BACKGROUND + 1));
            // A menu can be specified that all windows of this class would use by default,
            // but usually this is set for each window individually
            wndClassEx.lpszMenuName = nullptr;
            // The class name (see comments where this is initialized)
            wndClassEx.lpszClassName = i_mainWindowClassName;
        }
        // Register the class with Windows
        o_mainWindowClass = RegisterClassEx(&wndClassEx);
        if (o_mainWindowClass != NULL)
        {
            eae6320::Logging::OutputMessage("Registered main window class \"%s\"", i_mainWindowClassName);
            return eae6320::Results::success;
        }
        else
        {
            const auto errorMessage = eae6320::Windows::GetLastSystemError();
            EAE6320_ASSERTF(false, "Main window class registration failed: %s", errorMessage.c_str());
            eae6320::Logging::OutputError("Windows failed to register the main window class: %s", errorMessage.c_str());
            return eae6320::Results::Failure;
        }
    }

    eae6320::cResult FreeMainWindow(HWND& io_window)
    {
        if (DestroyWindow(io_window) != FALSE)
        {
            eae6320::Logging::OutputMessage("Destroyed main window");
            // The handle should have been set to NULL in OnMessageReceivedFromWindows() WM_NCDESTROY
            EAE6320_ASSERT(io_window == NULL);
            io_window = nullptr;
            return eae6320::Results::success;
        }
        else
        {
            const auto errorMessage = eae6320::Windows::GetLastSystemError();
            EAE6320_ASSERTF(false, "Main window wasn't destroyed: %s", errorMessage.c_str());
            eae6320::Logging::OutputError("Windows failed to destroy the main window: %s", errorMessage.c_str());
            return eae6320::Results::Failure;
        }
    }

    eae6320::cResult FreeMainWindowClass(const HINSTANCE i_thisInstanceOfTheApplication, ATOM& io_mainWindowClass)
    {
        if (UnregisterClass(MAKEINTATOM(io_mainWindowClass), i_thisInstanceOfTheApplication) != FALSE)
        {
            eae6320::Logging::OutputMessage("Unregistered main window class");
            io_mainWindowClass = NULL;
            return eae6320::Results::success;
        }
        else
        {
            const auto errorMessage = eae6320::Windows::GetLastSystemError();
            EAE6320_ASSERTF(false, "Main window class wasn't unregistered: %s", errorMessage.c_str());
            eae6320::Logging::OutputError("Windows failed to unregister the main window class: %s", errorMessage.c_str());
            return eae6320::Results::Failure;
        }
    }
}
