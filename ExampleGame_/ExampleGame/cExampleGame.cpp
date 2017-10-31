// Include Files
//==============

#include "cExampleGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Gameobject/cGameobject2D.h>
#include <Engine/Gameobject/cGameobject3D.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/MeshHelperStructs.h>
#include <Engine/Graphics/VertexFormats.h>

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
	// Predict 3D Gameobjects
	{
		for (size_t i = 0; i < s_3D_GameObject_Size; i++)
		{
			s_3D_GameObject[i]->PredictSimulationBasedOnElapsedTime(i_elapsedSecondCount_sinceLastSimulationUpdate);
		}
	}
	// Submit Clear Color to Graphics
	{
		Graphics::ColorFormats::sColor clearColor;
		if (!clearColor.SetColor(0.0f, 0.0f, 0.0f))
		{
			EAE6320_ASSERT(false);
			Logging::OutputError("All the SetColor parameters must be [0.0, 1.0]");
		}
		Graphics::SubmitClearColor(clearColor);
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
	// Creating all 3D gameobjects
	{
		{
			const Graphics::HelperStructs::sMeshData meshData(16, 5, 9);
			{
				auto& meshVertex = meshData.vertexData[0];
				meshVertex.x = 0.0f;
				meshVertex.y = 0.25f;
				meshVertex.z = 0.0f;
				meshVertex.r = 255;
				meshVertex.g = 0;
				meshVertex.b = 0;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[1];
				meshVertex.x = -0.25f;
				meshVertex.y = 0.0f;
				meshVertex.z = 0.0f;
				meshVertex.r = 0;
				meshVertex.g = 255;
				meshVertex.b = 0;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[2];
				meshVertex.x = 0.25f;
				meshVertex.y = 0.0;
				meshVertex.z = 0.0f;
				meshVertex.r = 0;
				meshVertex.g = 0;
				meshVertex.b = 255;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[3];
				meshVertex.x = 0.25f;
				meshVertex.y = -0.25f;
				meshVertex.z = 0.0f;
				meshVertex.r = 0;
				meshVertex.g = 255;
				meshVertex.b = 255;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[4];
				meshVertex.x = -0.25f;
				meshVertex.y = -0.25f;
				meshVertex.z = 0.0f;
				meshVertex.r = 255;
				meshVertex.g = 0;
				meshVertex.b = 255;
				meshVertex.a = 255;
			}
			{
				const auto meshIndex = reinterpret_cast<uint16_t*>(meshData.indexData);
				meshIndex[0] = 0;
				meshIndex[1] = 1;
				meshIndex[2] = 2;
				meshIndex[3] = 1;
				meshIndex[4] = 4;
				meshIndex[5] = 3;
				meshIndex[6] = 3;
				meshIndex[7] = 2;
				meshIndex[8] = 1;
			}

			Gameobject::cGameobject3D* gameobject3D;
			if (!((result = Gameobject::cGameobject3D::Load("fake_go3d1_path", gameobject3D, Math::sVector::zero, meshData, "fake_effect2_path", "mesh.busl", "mesh.busl", 0, Gameplay::DEFAULT_GAMEOBJECT_CONTROLLER))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_3D_GameObject.push_back(gameobject3D);
		}
		{
			const Graphics::HelperStructs::sMeshData meshData(16, 3, 3);
			{
				auto& meshVertex = meshData.vertexData[0];
				meshVertex.x = 0.0f;
				meshVertex.y = 0.25f;
				meshVertex.z = 0.0f;
				meshVertex.r = 255;
				meshVertex.g = 64;
				meshVertex.b = 128;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[1];
				meshVertex.x = -0.25f;
				meshVertex.y = 0.0f;
				meshVertex.z = 0.0f;
				meshVertex.r = 64;
				meshVertex.g = 255;
				meshVertex.b = 128;
				meshVertex.a = 255;
			}
			{
				auto& meshVertex = meshData.vertexData[2];
				meshVertex.x = 0.25f;
				meshVertex.y = 0.0;
				meshVertex.z = 0.0f;
				meshVertex.r = 255;
				meshVertex.g = 128;
				meshVertex.b = 64;
				meshVertex.a = 255;
			}
			{
				const auto meshIndex = reinterpret_cast<uint16_t*>(meshData.indexData);
				meshIndex[0] = 0;
				meshIndex[1] = 1;
				meshIndex[2] = 2;
			}

			Gameobject::cGameobject3D* gameobject3D;
			if (!((result = Gameobject::cGameobject3D::Load("fake_go3d1_path", gameobject3D, Math::sVector(0.75f, 0.75f, 0.0f), meshData, "fake_effect2_path", "mesh.busl", "mesh.busl", 0, Gameplay::NO_CONTROLLER))))
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
		/*{
			Gameobject::cGameobject2D* gameobject2D;
			if (!((result = Gameobject::cGameobject2D::Load("fake_go2d3_path", gameobject2D, 0, 0, 128, 128, Transform::BOTTOM_RIGHT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/sad.btf"))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_2D_GameObject.push_back(gameobject2D);
		}
		{
			Gameobject::cGameobject2D* gameobject2D;
			if (!((result = Gameobject::cGameobject2D::Load("fake_go2d4_path", gameobject2D, 0, 0, 128, 128, Transform::TOP_RIGHT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/smiling.btf", "data/Textures/sad.btf"))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_2D_GameObject.push_back(gameobject2D);
		}
		{
			Gameobject::cGameobject2D* gameobject2D;
			if (!((result = Gameobject::cGameobject2D::Load("fake_go2d5_path", gameobject2D, 0, 0, 128, 128, Transform::BOTTOM_LEFT, "fake_effect1_path", "sprite.shd", "sprite.shd", alphaTransperancyAndDrawingBothSidedTrianglesEnabled, "data/Textures/smiling.btf", "data/Textures/sad.btf"))))
			{
				EAE6320_ASSERT(false);
				goto OnExit;
			}
			s_2D_GameObject.push_back(gameobject2D);
		}*/
	}

OnExit:
	s_2D_GameObject_Size = s_2D_GameObject.size();
	s_3D_GameObject_Size = s_3D_GameObject.size();
	return result;
}

eae6320::cResult eae6320::cExampleGame::CleanUp()
{
	for (size_t i = 0; i < s_2D_GameObject_Size; i++)
	{
		// Clean up 2d gameobject
		s_2D_GameObject[i]->DecrementReferenceCount();
	}
	s_2D_GameObject.clear();
	for (size_t i = 0; i < s_3D_GameObject_Size; i++)
	{
		// Clean up 3d gameobject
		s_3D_GameObject[i]->DecrementReferenceCount();
	}
	s_3D_GameObject.clear();
	return Results::success;
}
