// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Gameobject/cGameobject2D.h>
#include <Engine/Gameobject/cGameobject3D.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Camera/Camera.h>
#include <Engine/Camera/cbCamera.h>
#include <Engine/Camera/cFirstPersonCamera.h>

#include <vector>

namespace
{
	std::vector<eae6320::Gameobject::cGameobject2D*> s_2D_GameObject;
	std::vector<eae6320::Gameobject::cGameobject3D*> s_3D_GameObject;
	auto s_isPaused = false;
	auto currentElapsedTime = 0.0f;
	size_t s_2D_GameObject_Size = 0;
	size_t s_3D_GameObject_Size = 0;
}

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cExampleGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::ESCAPE))
	{
		// Exit the application
		const auto result = Exit(EXIT_SUCCESS);
		EAE6320_ASSERT(result);
	}

	// Is the user pressing the P key?
	if (UserInput::IsKeyPressedOnce(UserInput::KeyCodes::P))
	{
		s_isPaused = !s_isPaused;
		s_isPaused ? SetSimulationRate(0.0f) : SetSimulationRate(1.0f);
	}
}

void eae6320::cExampleGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	currentElapsedTime += i_elapsedSecondCount_sinceLastUpdate;
	if (currentElapsedTime > 1.0f)
	{
		{
			auto const& gameObject2D = s_2D_GameObject[0];
			gameObject2D->m_useAlternateTexture = !gameObject2D->m_useAlternateTexture;
		}
		{
			auto const& gameObject2D = s_2D_GameObject[1];
			gameObject2D->m_useAlternateTexture = !gameObject2D->m_useAlternateTexture;
		}
		currentElapsedTime = 0.0f;
	}
}

void eae6320::cExampleGame::UpdateSimulationBasedOnInput()
{
	// Change current camera
	{
		Camera::ChangeCurrentCamera();
	}

	//Update current camera
	{
		Camera::GetCurrentCamera()->UpdateOrientation();
		Camera::GetCurrentCamera()->UpdatePosition();
	}

	// Update 3D Gameobjects
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			s_3D_GameObject[i]->UpdateBasedOnSimulationInput();
		}
	}
}
void eae6320::cExampleGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{

	//Update current camera
	{
		Camera::GetCurrentCamera()->UpdateOrientation(i_elapsedSecondCount_sinceLastUpdate);
		Camera::GetCurrentCamera()->UpdatePosition(i_elapsedSecondCount_sinceLastUpdate);
	}

	// Update 3D Gameobjects
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			s_3D_GameObject[i]->UpdateBasedOnSimulationTime(i_elapsedSecondCount_sinceLastUpdate);
		}
	}
}

void eae6320::cExampleGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	// Predict Camera
	{
		Camera::GetCurrentCamera()->PredictOrientation(i_elapsedSecondCount_sinceLastSimulationUpdate);
		Camera::GetCurrentCamera()->PredictPosition(i_elapsedSecondCount_sinceLastSimulationUpdate);
	}

	// Predict 3D Gameobjects
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			s_3D_GameObject[i]->PredictSimulationBasedOnElapsedTime(i_elapsedSecondCount_sinceLastSimulationUpdate);
		}
	}

	// Submit Clear Color
	{
		/*Graphics::ColorFormats::sColor clearColor;
		if (!clearColor.SetColor(0.0f, 0.0f, 0.0f))
		{
			EAE6320_ASSERT(false);
			Logging::OutputError("All the SetColor parameters must be [0.0, 1.0]");
		}*/
		Graphics::SubmitClearColor();
	}

	// Submit Clear Depth
	{
		//constexpr auto depth = 1.0f;
		Graphics::SubmitClearDepth();
	}

	// Submit Current Camera
	{
		Graphics::SubmitCamera(Camera::GetCurrentCamera());
	}

	// Submit 3D Gameobjects
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			Graphics::SubmitGameobject3D(s_3D_GameObject[i]);
		}
	}

	// Submit 2D Gameobjects
	{
		for (size_t i = 0; i < s_2D_GameObject_Size; i++)
		{
			Graphics::SubmitGameobject2D(s_2D_GameObject[i]);
		}
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::cExampleGame::Initialize()
{
	cResult result;
	// Create a camera
	{
		AddNewCamera(reinterpret_cast<Camera::cbCamera*>(Camera::cFirstPersonCamera::Initialize(Math::sVector(0.0f, 0.0f, 10.0f))));
	}
	// Creating all 3D gameobjects
	{
		constexpr auto depthBufferingEnabled = 0x02;
		{
			/*Gameobject::cGameobject3D* gameobject3D;
			if (!((result = Gameobject::cGameobject3D::Load("fake_go3d1_path", gameobject3D, Math::sVector::zero, "data/Meshes/cube.bmf", "fake_effect2_path", "mesh.busl", "mesh.busl", depthBufferingEnabled, Gameplay::DEFAULT_GAMEOBJECT_CONTROLLER))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_3D_GameObject.push_back(gameobject3D);*/
		}
		{
			Gameobject::cGameobject3D* gameobject3D;
			if (!((result = Gameobject::cGameobject3D::Load("fake_go3d2_path", gameobject3D, Math::sVector(0.0f, -1.0f, 0.0f), "data/Meshes/plane2.bmf", "fake_effect2_path", "mesh.busl", "mesh.busl", depthBufferingEnabled, Gameplay::NO_CONTROLLER))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_3D_GameObject.push_back(gameobject3D);
		}
	}
	// Creating all 2D gameobjects
	{
		constexpr auto alphaTransperancyAndDrawingBothSidedTrianglesEnabled = 0x05;
		{
			Gameobject::cGameobject2D* gameobject2D;
			if (!((result = Gameobject::cGameobject2D::Load("fake_go2d1_path", gameobject2D, 0, 0, 128, 128, Transform::TOP_LEFT, "fake_effect1_path", "sprite.busl", "sprite.busl", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/happy.btf", "data/Textures/smiling.btf"))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_2D_GameObject.push_back(gameobject2D);
		}
		{
			Gameobject::cGameobject2D* gameobject2D;
			if (!((result = Gameobject::cGameobject2D::Load("fake_go2d2_path", gameobject2D, 0, 0, 128, 128, Transform::BOTTOM_RIGHT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/sad.btf", "data/Textures/happy.btf"))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_2D_GameObject.push_back(gameobject2D);
		}
	}

OnExit:
	s_2D_GameObject_Size = s_2D_GameObject.size();
	s_3D_GameObject_Size = s_3D_GameObject.size();
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	// Clean up cameras
	{
		Camera::CleanUp();
	}

	// Clean up 2d gameobject
	{
		for (size_t i = 0; i < s_2D_GameObject_Size; i++)
		{
			s_2D_GameObject[i]->DecrementReferenceCount();
		}
		s_2D_GameObject.clear();
	}

	// Clean up 3d gameobject
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			s_3D_GameObject[i]->DecrementReferenceCount();
		}
		s_3D_GameObject.clear();
	}

	return Results::success;
}
