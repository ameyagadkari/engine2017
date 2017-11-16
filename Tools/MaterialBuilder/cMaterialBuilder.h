/*
This class builds the materials
*/

#ifndef EAE6320_CEFFECTBUILDER_H
#define EAE6320_CEFFECTBUILDER_H

// Include Files
//==============

#include <Tools/AssetBuildLibrary/cbBuilder.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMaterialBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) override;
		};
	}
}

#endif	// EAE6320_CEFFECTBUILDER_H
