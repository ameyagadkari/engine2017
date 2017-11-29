--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	materials =
	{
		"Materials/ball.tmaf",
		"Materials/capsule.tmaf",
		"Materials/capsule2.tmaf",
		"Materials/floor.tmaf",
	},
	meshes =
	{
		"Meshes/ball.tmf",
		"Meshes/capsule.tmf",
		"Meshes/floor.tmf",
	},
	effects =
	{
		"Effects/sprite.tef",
	},
	shaders =
	{
		{ path = "Shaders/Vertex/vertexInputLayout_sprite.tusl", arguments = { "vertex" } },
	},
	textures =
	{
		"Textures/Sprites/happy.png",
		"Textures/Sprites/sad.png",
		"Textures/Sprites/smiling.png",
	},
}
