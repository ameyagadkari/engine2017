#ifndef EAE6320_GRAPHICS_CRENDERSTATE_INL
#define EAE6320_GRAPHICS_CRENDERSTATE_INL

// Include Files
//==============

// #include "cRenderState.h"

// Interface
//==========

// Alpha Transparency
inline bool eae6320::Graphics::RenderStates::IsAlphaTransparencyEnabled(const uint8_t i_renderStateBits)
{
    return (i_renderStateBits & AlphaTransparency) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableAlphaTransparency(uint8_t& io_renderStateBits)
{
    io_renderStateBits |= AlphaTransparency;
}
inline void eae6320::Graphics::RenderStates::DisableAlphaTransparency(uint8_t& io_renderStateBits)
{
    io_renderStateBits &= ~AlphaTransparency;
}

// Depth Testing
inline bool eae6320::Graphics::RenderStates::IsDepthTestingEnabled(const uint8_t i_renderStateBits)
{
    return (i_renderStateBits & DepthTesting) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableDepthTesting(uint8_t& io_renderStateBits)
{
    io_renderStateBits |= DepthTesting;
}
inline void eae6320::Graphics::RenderStates::DisableDepthTesting(uint8_t& io_renderStateBits)
{
    io_renderStateBits &= ~DepthTesting;
}

// Depth Writing
inline bool eae6320::Graphics::RenderStates::IsDepthWritingEnabled(const uint8_t i_renderStateBits)
{
    return (i_renderStateBits & DepthWriting) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableDepthWriting(uint8_t& io_renderStateBits)
{
    io_renderStateBits |= DepthWriting;
}
inline void eae6320::Graphics::RenderStates::DisableDepthWriting(uint8_t& io_renderStateBits)
{
    io_renderStateBits &= ~DepthWriting;
}

// Draw Both Triangle Sides
inline bool eae6320::Graphics::RenderStates::ShouldBothTriangleSidesBeDrawn(const uint8_t i_renderStateBits)
{
    return (i_renderStateBits & DrawBothTriangleSides) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(uint8_t& io_renderStateBits)
{
    io_renderStateBits |= DrawBothTriangleSides;
}
inline void eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(uint8_t& io_renderStateBits)
{
    io_renderStateBits &= ~DrawBothTriangleSides;
}

// Draw Triangles In Wire Frame Mode
inline bool eae6320::Graphics::RenderStates::IsWireFrameModeEnabled(const uint8_t i_renderStateBits)
{
    return (i_renderStateBits & DrawWireFrame) != 0;
}
inline void eae6320::Graphics::RenderStates::EnableWireFrameMode(uint8_t& io_renderStateBits)
{
    io_renderStateBits |= DrawWireFrame;
}
inline void eae6320::Graphics::RenderStates::DisableWireFrameMode(uint8_t& io_renderStateBits)
{
    io_renderStateBits &= ~DrawWireFrame;
}

// Access
//-------

inline bool eae6320::Graphics::cRenderState::IsAlphaTransparencyEnabled() const
{
    return RenderStates::IsAlphaTransparencyEnabled(m_bits);
}

inline bool eae6320::Graphics::cRenderState::IsDepthTestingEnabled() const
{
    return RenderStates::IsDepthTestingEnabled(m_bits);
}

inline bool eae6320::Graphics::cRenderState::IsDepthWritingEnabled() const
{
    return RenderStates::IsDepthWritingEnabled(m_bits);
}

inline bool eae6320::Graphics::cRenderState::ShouldBothTriangleSidesBeDrawn() const
{
    return RenderStates::ShouldBothTriangleSidesBeDrawn(m_bits);
}

inline bool eae6320::Graphics::cRenderState::IsWireFrameModeEnabled() const
{
    return RenderStates::IsWireFrameModeEnabled(m_bits);
}

inline uint8_t eae6320::Graphics::cRenderState::GetRenderStateBits() const
{
    return m_bits;
}

#endif    // EAE6320_GRAPHICS_CRENDERSTATE_INL
