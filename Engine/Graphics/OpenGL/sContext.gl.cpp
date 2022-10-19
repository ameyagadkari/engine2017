// Include Files
//==============

#include "../sContext.h"
#include "../ColorFormats.h"
#include "../Graphics.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Windows/Functions.h>

#include <External/DirectXTex/Includes.h>
#include <External/OpenGlExtensions/OpenGlExtensions.h>

#include <sstream>
#include <string>

// Helper Function Declarations
//=============================

namespace
{
    eae6320::cResult CreateRenderingContext();
}

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::sContext::Initialize(const sInitializationParameters& i_initializationParameters)
{
    auto result = Results::success;

    windowBeingRenderedTo = i_initializationParameters.mainWindow;
    resolutionWidth = i_initializationParameters.resolutionWidth;
    resolutionHeight = i_initializationParameters.resolutionHeight;

    // Load any required OpenGL extensions
    {
        std::string errorMessage;
        if (!((result = OpenGlExtensions::Load(&errorMessage))))
        {
            EAE6320_ASSERTF(false, errorMessage.c_str());
            Logging::OutputError(errorMessage.c_str());
            goto OnExit;
        }
    }
    // Create an OpenGL rendering context
    if (!((result = CreateRenderingContext())))
    {
        EAE6320_ASSERT(false);
        goto OnExit;
    }

OnExit:

    return result;
}

eae6320::cResult eae6320::Graphics::sContext::CleanUp()
{
    auto result = Results::success;

    if (openGlRenderingContext != nullptr)
    {
        if (wglMakeCurrent(deviceContext, nullptr) != FALSE)
        {
            if (wglDeleteContext(openGlRenderingContext) == FALSE)
            {
                if (result)
                {
                    result = Results::Failure;
                }
                const auto windowsErrorMessage = Windows::GetLastSystemError();
                EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
                Logging::OutputError("Windows failed to delete the OpenGL rendering context: %s", windowsErrorMessage.c_str());
            }
        }
        else
        {
            if (result)
            {
                result = Results::Failure;
            }
            const auto windowsErrorMessage = Windows::GetLastSystemError();
            EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
            Logging::OutputError("Windows failed to unset the current OpenGL rendering context: %s", windowsErrorMessage.c_str());
        }
        openGlRenderingContext = nullptr;
    }

    if (deviceContext != nullptr)
    {
        // The documentation says that this call isn't necessary when CS_OWNDC is used
        ReleaseDC(windowBeingRenderedTo, deviceContext);
        deviceContext = nullptr;
    }

    windowBeingRenderedTo = nullptr;

    return result;
}

// Render
//-------

void eae6320::Graphics::sContext::ClearImageBuffer(const ColorFormats::sColor i_color) const
{
    // Black is usually used
    {
        glClearColor(i_color.R(), i_color.G(), i_color.B(), i_color.A());
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
    }
    {
        constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
        glClear(clearColor);
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
    }
}

void eae6320::Graphics::sContext::ClearDepthBuffer(const float i_depth) const
{
    {
        glDepthMask(GL_TRUE);
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
        glClearDepth(i_depth);
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
    }
    {
        constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
        glClear(clearDepth);
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
    }
}

void eae6320::Graphics::sContext::BufferSwap() const
{
    EAE6320_ASSERT(deviceContext != nullptr);
    const auto glResult = SwapBuffers(deviceContext);
    EAE6320_ASSERT(glResult != FALSE);
}

// User actions
//-------------
void eae6320::Graphics::sContext::GetRawImageFromBackBuffer(Platform::sDataFromFile& o_rawImageData) const
{
    // Fetch data from the default frame buffer
    {
        constexpr GLint originX = 0;
        constexpr GLint originY = 0;
        constexpr GLenum backBufferFormat = GL_RGBA;
        constexpr GLenum rawImageBufferDataType = GL_UNSIGNED_BYTE;
        glReadPixels(originX, originY, resolutionWidth, resolutionHeight, backBufferFormat, rawImageBufferDataType, o_rawImageData.data);
        EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
    }

    // Populate the image struct manually
    DirectX::Image image = {};
    {
        image.width = resolutionWidth;
        image.height = resolutionHeight;
        image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        const size_t bytesPerPixel = DirectX::BitsPerPixel(image.format) / 8;
        image.rowPitch = image.width * bytesPerPixel;
        image.slicePitch = image.rowPitch * image.height;
        image.pixels = static_cast<uint8_t*>(o_rawImageData.data);
    }

    // OpenGL images are upside-down from what DirectXTex expects so flip it
    DirectX::ScratchImage flippedImage = {};
    {
        constexpr DWORD flipVertically = DirectX::TEX_FR_FLIP_VERTICAL;
        const auto d3DResult = DirectX::FlipRotate(image, flipVertically, flippedImage);
        if (SUCCEEDED(d3DResult))
        {
            EAE6320_ASSERTF(flippedImage.GetImageCount() == 1, "There shouldn't be more than one image (mip) in back buffer since MSAA is disabled");
            constexpr unsigned int imageIndex = 0;
            const DirectX::Image& backBufferImage = flippedImage.GetImages()[imageIndex];
            EAE6320_ASSERTF(o_rawImageData.size == backBufferImage.slicePitch, "The slicePitch is the whole texture for a 2D image buffer");
            memcpy_s(o_rawImageData.data, o_rawImageData.size, backBufferImage.pixels, backBufferImage.slicePitch);
        }
        else
        {
            EAE6320_ASSERTF(false, "Couldn't flip the source image vertically (HRESULT %#010x)", d3DResult);
            eae6320::Logging::OutputError("DirectXTex failed to flip the source image vertically (HRESULT %#010x)", d3DResult);
        }
    }
}

// Helper Function Declarations
//=============================

namespace
{
    eae6320::cResult CreateRenderingContext()
    {
        auto& g_context = eae6320::Graphics::sContext::g_context;

        // Get the device context
        {
            g_context.deviceContext = GetDC(g_context.windowBeingRenderedTo);
            if (g_context.deviceContext == nullptr)
            {
                EAE6320_ASSERTF(false, "Couldn't get device context");
                eae6320::Logging::OutputError("Windows failed to get the device context");
                return eae6320::Results::Failure;
            }
        }
        // Set the pixel format for the window
        // (This can only be done _once_ for a given window)
        {
            // Get the ID of the desired pixel format
            int pixelFormatId;
            {
                // Create a key/value list of attributes that the pixel format should have
                constexpr int desiredAttributes[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                    WGL_COLOR_BITS_ARB, 24,
                    WGL_RED_BITS_ARB, 8,
                    WGL_GREEN_BITS_ARB, 8,
                    WGL_BLUE_BITS_ARB, 8,
                    WGL_DEPTH_BITS_ARB, 24,
                    WGL_STENCIL_BITS_ARB, 8,
                    // NULL terminator
                    NULL
                };
                constexpr float* const noFloatAttributes = nullptr;
                constexpr unsigned int onlyReturnBestMatch = 1;
                unsigned int returnedFormatCount;
                if (wglChoosePixelFormatARB(g_context.deviceContext, desiredAttributes, noFloatAttributes, onlyReturnBestMatch,
                    &pixelFormatId, &returnedFormatCount) != FALSE)
                {
                    if (returnedFormatCount == 0)
                    {
                        EAE6320_ASSERTF(false, "Couldn't find a pixel format");
                        eae6320::Logging::OutputError("Windows couldn't find a pixel format that satisfied the desired attributes");
                        return eae6320::Results::Failure;
                    }
                }
                else
                {
                    const auto windowsErrorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
                    eae6320::Logging::OutputError("Windows failed to choose the closest pixel format: %s", windowsErrorMessage.c_str());
                    return eae6320::Results::Failure;
                }
            }
            // Set it
            {
                PIXELFORMATDESCRIPTOR pixelFormatDescriptor{};
                {
                    // I think that the values of this struct are ignored
                    // and unnecessary when using wglChoosePixelFormatARB() instead of ChoosePixelFormat(),
                    // but the documentation is very unclear and so filling it in seems the safest bet
                    pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
                    pixelFormatDescriptor.nVersion = 1;
                    pixelFormatDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
                    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
                    pixelFormatDescriptor.cColorBits = 24;
                    pixelFormatDescriptor.cDepthBits = 24;
                    pixelFormatDescriptor.cStencilBits = 8;
                    pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
                }
                if (SetPixelFormat(g_context.deviceContext, pixelFormatId, &pixelFormatDescriptor) == FALSE)
                {
                    const auto windowsErrorMessage = eae6320::Windows::GetLastSystemError();
                    EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
                    eae6320::Logging::OutputError("Windows couldn't set the desired pixel format: %s", windowsErrorMessage.c_str());
                    return eae6320::Results::Failure;
                }
            }
        }
        // Create an OpenGL rendering context and make it current
        {
            // Create the context
            {
                // Create a key/value list of attributes that the context should have
                constexpr int desiredAttributes[] =
                {
                    // Request at least version 4.2
                    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                    WGL_CONTEXT_MINOR_VERSION_ARB, 2,
                    // Request only "core" functionality and not "compatibility"
                    // (i.e. only use modern features of version 4.2)
                    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
                    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
                    // NULL terminator
                    NULL
                };
                constexpr HGLRC noSharedContexts = nullptr;
                g_context.openGlRenderingContext = wglCreateContextAttribsARB(g_context.deviceContext, noSharedContexts, desiredAttributes);
                if (g_context.openGlRenderingContext == nullptr)
                {
                    DWORD errorCode;
                    const auto windowsErrorMessage = eae6320::Windows::GetLastSystemError(&errorCode);
                    std::ostringstream errorMessage;
                    errorMessage << "Windows failed to create an OpenGL rendering context: ";
                    if ((errorCode == ERROR_INVALID_VERSION_ARB)
                        || (HRESULT_CODE(errorCode) == ERROR_INVALID_VERSION_ARB))
                    {
                        errorMessage << "The requested version number is invalid";
                    }
                    else if ((errorCode == ERROR_INVALID_PROFILE_ARB)
                        || (HRESULT_CODE(errorCode) == ERROR_INVALID_PROFILE_ARB))
                    {
                        errorMessage << "The requested profile is invalid";
                    }
                    else
                    {
                        errorMessage << windowsErrorMessage;
                    }
                    EAE6320_ASSERTF(false, errorMessage.str().c_str());
                    eae6320::Logging::OutputError(errorMessage.str().c_str());

                    return eae6320::Results::Failure;
                }
            }
            // Set it as the rendering context of this thread
            if (wglMakeCurrent(g_context.deviceContext, g_context.openGlRenderingContext) == FALSE)
            {
                const auto windowsErrorMessage = eae6320::Windows::GetLastSystemError();
                EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
                eae6320::Logging::OutputError("Windows failed to set the current OpenGL rendering context: %s",
                    windowsErrorMessage.c_str());
                return eae6320::Results::Failure;
            }
        }

        return eae6320::Results::success;
    }
}
