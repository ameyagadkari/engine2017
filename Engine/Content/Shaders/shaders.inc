/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Types
//======

#if defined( EAE6320_PLATFORM_D3D )

	#define vec2 float2
	#define vec3 float3
	#define vec4 float4
	#define mat2 float2x2
	#define mat3 float3x3
	#define mat4 float4x4
	
	#define gl_Position o_position

#elif defined( EAE6320_PLATFORM_GL )

	#define float2 vec2
	#define float3 vec3
	#define float4 vec4
	#define float2x2 mat2
	#define float3x3 mat3
	#define float4x4 mat4
	
	#define o_position gl_Position

#endif

// Operators
//==========
#if defined( EAE6320_PLATFORM_GL )

	#define mul(a, b) a * b 

#endif

// Texture Objects
//================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareTexture2DObject( i_name, i_id ) Texture2D i_name : register( t##i_id );

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareTexture2DObject( i_name, i_id ) layout( binding = i_id ) uniform sampler2D i_name;

#endif


// Sampler State Objects
//======================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareSamplerStateObject( i_name, i_id ) SamplerState i_name : register( s##i_id );

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareSamplerStateObject( i_name, i_id )

#endif

DeclareTexture2DObject( g_color_texture, 0 )

// Sampled Color
//==============

#if defined( EAE6320_PLATFORM_D3D )

	#define SampleColor( i_texture2D, i_samplerstate, i_uv ) i_texture2D.Sample( i_samplerstate, i_uv )

#elif defined( EAE6320_PLATFORM_GL )

	#define SampleColor( i_texture2D, i_samplerstate, i_uv ) texture2D( i_texture2D, i_uv )

#endif

DeclareSamplerStateObject( g_color_samplerState, 0 )

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

#endif

DeclareConstantBuffer( g_constantBuffer_perFrame, 0)
{
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
};

DeclareConstantBuffer( g_constantBuffer_perMaterial, 1)
{
	float4 g_color;
};

DeclareConstantBuffer( g_constantBuffer_perDrawCall, 2)
{
	// This is a placeholder to prevent an empty constant buffer declaration
	float4 DUMMY;
};
