// Include Files
//==============

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>&)
{
	cResult result;
	constexpr auto shouldFunctionFailIfTargetAlreadyExists = false;
	constexpr auto shouldTargetFileTimeBeModified = true;
	std::string errorMessage;
	if (!((result = eae6320::Platform::CopyFile(m_path_source, m_path_target, shouldFunctionFailIfTargetAlreadyExists, shouldTargetFileTimeBeModified, &errorMessage))))
	{
		OutputErrorMessage("The \"%s\" couldn't be copied to \"%s\": %s", m_path_source, m_path_target, errorMessage.c_str());
		goto OnExit;
	}
OnExit:
	return result;
}