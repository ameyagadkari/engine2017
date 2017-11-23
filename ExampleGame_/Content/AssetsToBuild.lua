--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	--[[materials =
	{
		"Materials/ball.tmaf",
		"Materials/floor.tmaf",
	},
	meshes =
	{
		"Meshes/ball.tmf",
		"Meshes/floor.tmf",
	},
	effects =
	{
		"Effects/mesh.tef",
		"Effects/sprite.tef",
	},]]
	shaders =
	{
		{ path = "Shaders/Vertex/mesh.tusl", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/mesh.tusl", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/sprite.tusl", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/sprite.tusl", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.tusl", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/vertexInputLayout_sprite.tusl", arguments = { "vertex" } },
	},
	--[[textures =
	{
		"Textures/ball.jpg",
		"Textures/floor.jpg",
		"Textures/happy.png",
		"Textures/sad.png",
		"Textures/smiling.png",
	},]]
}
