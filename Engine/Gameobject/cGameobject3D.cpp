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
	m_transform(i_position, Math::cQuaternion()),
	m_controller(nullptr)
{
	m_predictionTransform.position = m_transform.position;
	m_predictionTransform.orientation = m_transform.orientation;
	switch (i_controllerType)
	{
	case Gameplay::DEFAULT_GAMEOBJECT_CONTROLLER:
		m_controller = reinterpret_cast<Gameplay::cbController*>(Gameplay::DefaultController::Initialize());
		break;
	default:;
	}
}

eae6320::cResult eae6320::Gameobject::cGameobject3D::Load(const char* const i_path, cGameobject3D*& o_gameobject3D, const Math::sVector& i_position, char const * const i_meshPath, char const * const i_materialPath, const Gameplay::eControllerType i_controllerType)
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

	// Load the material
	if (!((result = Graphics::cMaterial::s_manager.Load(i_materialPath, newGameobject3D->m_material))))
	{
		EAE6320_ASSERTF(false, "Loading of material failed: \"%s\"", i_materialPath);
		goto OnExit;
	}
	newGameobject3D->m_pMaterial = Graphics::cMaterial::s_manager.Get(newGameobject3D->m_material);

	// Load the mesh
	{
		if (!((result = Graphics::cMesh::s_manager.Load(i_meshPath, newGameobject3D->m_mesh))))
		{
			EAE6320_ASSERTF(false, "Loading of mesh failed");
			goto OnExit;
		}
	}
	newGameobject3D->m_pMesh = Graphics::cMesh::s_manager.Get(newGameobject3D->m_mesh);

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

	// Material Clean Up
	if (m_material)
	{
		const auto localResult = Graphics::cMaterial::s_manager.Release(m_material);
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
		const auto localResult = Graphics::cMesh::s_manager.Release(m_mesh);
		if (!localResult)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = localResult;
			}
		}
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
		m_predictionTransform.position = m_transform.position;
		m_predictionTransform.orientation = m_transform.orientation;
		m_controller->UpdatePosition(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
		m_controller->UpdateOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate, m_predictionTransform);
	}
}

// Render
//-------

void eae6320::Gameobject::cGameobject3D::BindAndDraw() const
{
	EAE6320_ASSERT(m_pMaterial);
	m_pMaterial->Bind();
	EAE6320_ASSERT(m_pMesh);
	m_pMesh->Draw();
}

