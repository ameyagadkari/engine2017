--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	meshes =
	{
		EAE6320_TODO
	},
	shaders =
	{
		-- EAE6320_TODO:
		-- These are some examples showing how to use the optional argument,
		-- but with my disk layout and file extension.
		-- You need to update the paths to match your content
		-- and then add the other shaders that you use.
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
		{ path = "Shaders/Vertex/vertexInputLayout_sprite.shader", arguments = { "vertex" } },
	},
	textures =
	{
		-- EAE6320_TODO:
		-- These are some examples with my disk layout.
		-- You need to update the paths to match your content
		-- and then add the other textures that you use.
		"Images/eaeAlien.png",
		"Images/eaeGamePad.png",
	},
}
