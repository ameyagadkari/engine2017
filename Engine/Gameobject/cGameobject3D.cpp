// Include Files
//==============

#include "cGameobject3D.h"
#include <Engine/Graphics/cMesh.h>
#include <ExampleGame_/Gameplay/cDefaultGameobjectController.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Gameobject::cGameobject3D::cGameobject3D(const Math::sVector& i_position, const Gameplay::eControllerType i_controllerType)
	:
	m_transform(i_position, Math::sVector::zero),
	m_controller(nullptr),
	m_mesh(nullptr)
{
	m_predictionTransform.position = m_transform.position;
	m_predictionTransform.orientation = m_transform.orientationQuaternion;
	switch (i_controllerType)
	{
	case Gameplay::DEFAULT_GAMEOBJECT_CONTROLLER:
		m_controller = reinterpret_cast<Gameplay::cbController*>(Gameplay::DefaultController::Initialize());
		break;
	default:;
	}
}

eae6320::cResult eae6320::Gameobject::cGameobject3D::Load(const char* const i_path, cGameobject3D*& o_gameobject3D, const Math::sVector& i_position, const Graphics::HelperStructs::sMeshData& i_meshData, char* const i_effectPath, const std::string& i_vertexShaderName, const std::string& i_fragmentShaderName, const uint8_t i_renderState, const Gameplay::eControllerType i_controllerType)
{
	auto result = Results::success;

	//Platform::sDataFromFile dataFromFile;

	cGameobject3D* newGameobject3D = nullptr;

	/*// Load the binary data
	{
	std::string errorMessage;
	if (!(result = Platform::LoadBinaryFile(i_path, dataFromFile, &errorMessage)))
	{
	EAE6320_ASSERTF(false, errorMessage.c_str());
	Logging::OutputError("Failed to load shader from file %s: %s", i_path, errorMessage.c_str());
	goto OnExit;
	}
	}*/

	// Allocate a new GameObject 3D
	{
		newGameobject3D = new (std::nothrow) cGameobject3D(i_position, i_controllerType);
		if (!newGameobject3D)
		{
			result = Results::outOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect %s", i_path);
			Logging::OutputError("Failed to allocate memory for the effect %s", i_path);
			goto OnExit;
		}
	}
	//if (!(result = newEffect->Initialize(i_path, dataFromFile)))

	// Load the effect
	if (!((result = Graphics::cEffect::s_manager.Load(i_effectPath, newGameobject3D->m_effect, i_vertexShaderName, i_fragmentShaderName, i_renderState))))
	{
		EAE6320_ASSERTF(false, "Loading of effect failed: \"%s\"", i_effectPath);
		goto OnExit;
	}

	// Load the mesh
	{
		if (!((result = Graphics::cMesh::Load(newGameobject3D->m_mesh, i_meshData))))
		{
			EAE6320_ASSERTF(false, "Loading of mesh failed");
			goto OnExit;
		}
	}

OnExit:

	if (result)
	{
		EAE6320_ASSERT(newGameobject3D);
		o_gameobject3D = newGameobject3D;
	}
	else
	{
		if (newGameobject3D)
		{
			newGameobject3D->DecrementReferenceCount();
			newGameobject3D = nullptr;
		}
		o_gameobject3D = nullptr;
	}

	//dataFromFile.Free();

	return result;
}

eae6320::cResult eae6320::Gameobject::cGameobject3D::CleanUp()
{
	auto result = Results::success;

	// Effect Clean Up
	if (m_effect)
	{
		const auto localResult = Graphics::cEffect::s_manager.Release(m_effect);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
	}

	// Mesh Clean Up
	if (m_mesh)
	{
		m_mesh->DecrementReferenceCount();
	}

	// Controller Clean Up
	if (m_controller)
	{
		delete m_controller;
	}

	return result;
}


// Update
//-------

void eae6320::Gameobject::cGameobject3D::UpdateBasedOnSimulationInput() const
{
	if (m_controller)
	{
		m_controller->UpdatePosition(m_transform);
		m_controller->UpdateOrientation(m_transform);
	}
}

void eae6320::Gameobject::cGameobject3D::UpdateBasedOnSimulationTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	if (m_controller)
	{
		m_controller->UpdatePosition(i_elapsedSecondCount_sinceLastUpdate, m_transform);
		m_controller->UpdateOrientation(i_elapsedSecondCount_sinceLastUpdate, m_transform);
	}
}

void eae6320::Gameobject::cGameobject3D::PredictSimulationBasedOnElapsedTime(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	if (m_controller)
	{
		m_controller->UpdatePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
		m_controller->UpdateOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
		m_transform.position = m_predictionTransform.position;
		m_transform.orientationQuaternion = m_predictionTransform.orientation;
	}
}

// Render
//-------

void eae6320::Gameobject::cGameobject3D::BindAndDraw() const
{
	Graphics::cEffect::s_manager.Get(m_effect)->Bind();
	m_mesh->Draw();
}
