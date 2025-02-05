// Include Files
//==============

#include "../cEffect.h"

// Implementation
//===============

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::cEffect::InitializePlatformSpecific()
{
    auto result = Results::success;

    // Create a program
    {
        m_programId = glCreateProgram();
        const auto errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
            Logging::OutputError("OpenGL failed to create a program: %s",
                reinterpret_cast<const char*>(gluErrorString(errorCode)));
            goto OnExit;
        }
        if (m_programId == 0)
        {
            result = Results::Failure;
            EAE6320_ASSERT(false);
            Logging::OutputError("OpenGL failed to create a program");
            goto OnExit;
        }
    }
    // Attach the shaders to the program
    {
        // Vertex
        {
            glAttachShader(m_programId, eae6320::Graphics::cShader::s_manager.Get(m_vertexShader)->m_shaderId);
            const auto errorCode = glGetError();
            if (errorCode != GL_NO_ERROR)
            {
                result = Results::Failure;
                EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
                    reinterpret_cast<const char*>(gluErrorString(errorCode)));
                goto OnExit;
            }
        }
        // Fragment
        {
            glAttachShader(m_programId, cShader::s_manager.Get(m_fragmentShader)->m_shaderId);
            const auto errorCode = glGetError();
            if (errorCode != GL_NO_ERROR)
            {
                result = Results::Failure;
                EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
                    reinterpret_cast<const char*>(gluErrorString(errorCode)));
                goto OnExit;
            }
        }
    }
    // Link the program
    {
        glLinkProgram(m_programId);
        const auto errorCode = glGetError();
        if (errorCode == GL_NO_ERROR)
        {
            // Get link info
            // (this won't be used unless linking fails
            // but it can be useful to look at when debugging)
            std::string linkInfo;
            {
                GLint infoSize;
                glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoSize);
                const auto errorCode = glGetError();
                if (errorCode == GL_NO_ERROR)
                {
                    const struct sLogInfo
                    {
                        GLchar* memory;
                        explicit sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
                        ~sLogInfo() { if (memory) free(memory); }
                    } info(static_cast<size_t>(infoSize));
                    GLsizei* const dontReturnLength = nullptr;
                    glGetProgramInfoLog(m_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
                    const auto errorCode = glGetError();
                    if (errorCode == GL_NO_ERROR)
                    {
                        linkInfo = info.memory;
                    }
                    else
                    {
                        result = Results::Failure;
                        EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                        Logging::OutputError("OpenGL failed to get link info of the program: %s",
                            reinterpret_cast<const char*>(gluErrorString(errorCode)));
                        goto OnExit;
                    }
                }
                else
                {
                    result = Results::Failure;
                    EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                    Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
                        reinterpret_cast<const char*>(gluErrorString(errorCode)));
                    goto OnExit;
                }
            }
            // Check to see if there were link errors
            GLint didLinkingSucceed;
            {
                glGetProgramiv(m_programId, GL_LINK_STATUS, &didLinkingSucceed);
                const auto errorCode = glGetError();
                if (errorCode == GL_NO_ERROR)
                {
                    if (didLinkingSucceed == GL_FALSE)
                    {
                        result = Results::Failure;
                        EAE6320_ASSERTF(false, linkInfo.c_str());
                        Logging::OutputError("The program failed to link: %s",
                            linkInfo.c_str());
                        goto OnExit;
                    }
                }
                else
                {
                    result = Results::Failure;
                    EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                    Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
                        reinterpret_cast<const char*>(gluErrorString(errorCode)));
                    goto OnExit;
                }
            }
        }
        else
        {
            result = Results::Failure;
            EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
            Logging::OutputError("OpenGL failed to link the program: %s",
                reinterpret_cast<const char*>(gluErrorString(errorCode)));
            goto OnExit;
        }
    }

OnExit:

    if (!result)
    {
        if (m_programId != 0)
        {
            glDeleteProgram(m_programId);
            const auto errorCode = glGetError();
            if (errorCode != GL_NO_ERROR)
            {
                result = Results::Failure;
                EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
                Logging::OutputError("OpenGL failed to delete the program: %s",
                    reinterpret_cast<const char*>(gluErrorString(errorCode)));
            }
            m_programId = 0;
        }
    }

    return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUpPlatformSpecific()
{
    auto result = Results::success;
    // Program clean up
    if (m_programId != 0)
    {
        glDeleteProgram(m_programId);
        const auto errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            if (result)
            {
                result = Results::Failure;
            }
            EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
            Logging::OutputError("OpenGL failed to delete the program: %s",
                reinterpret_cast<const char*>(gluErrorString(errorCode)));
        }
        m_programId = 0;
    }
    return result;
}

// Render
//-------

void eae6320::Graphics::cEffect::BindPlatformSpecific() const
{
    EAE6320_ASSERT(m_programId != 0);
    glUseProgram(m_programId);
    EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
}