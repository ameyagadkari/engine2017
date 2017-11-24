--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	materials =
	{
		"Materials/ball.tmaf",
		"Materials/floor.tmaf",
		"Materials/unlit.tmaf",
	},
	meshes =
	{
		"Meshes/ball.tmf",
		"Meshes/floor.tmf",
	},
	effects =
	{
		"Effects/sprite.tef",
	},
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.tusl", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/vertexInputLayout_sprite.tusl", arguments = { "vertex" } },
	},
	textures =
	{
		"Textures/Sprites/happy.png",
		"Textures/Sprites/sad.png",
		"Textures/Sprites/smiling.png",
	},
}
