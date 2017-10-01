/*
	This class builds shaders
*/

#ifndef EAE6320_CSHADERBUILDER_H
#define EAE6320_CSHADERBUILDER_H

// Include Files
//==============

#include <Tools/AssetBuildLibrary/cbBuilder.h>

#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cShader.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cShaderBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) override;

			// Implementation
			//===============

			// Build
			//------

			cResult Build(const Graphics::ShaderTypes::eType i_shaderType, const std::vector<std::string>& i_arguments) const;
		};
	}
}

#endif	// EAE6320_CSHADERBUILDER_H
