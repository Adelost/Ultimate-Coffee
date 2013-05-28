#pragma once
namespace Enum
{
	enum ColorScheme
	{
		RGB,
		CYM
	};

	enum CommandType
	{
		TRANSLATE_SCENE_ENTITY,
		ROTATE_SCENE_ENTITY,
		SCALE_SCENE_ENTITY,
		CHANGEBACKBUFFERCOLOR,
		SKYBOX,
		CREATE_ENTITY,
		REMOVE_ENTITY
	};

	enum ToolType
	{
		Tool_None,
		Tool_Selection,
		Tool_Translate,
		Tool_Rotate,
		Tool_Scale,
		Tool_Geometry,
		Tool_Entity
	};

	enum QtKeyPressType
	{
		QtKeyPress_NoButton = 0,
		QtKeyPress_MouseLeft,
		QtKeyPress_MouseRight
	};

	enum EntityType
	{
		Entity_Empty,
		Entity_Sky,
		Entity_Mesh,
		Entity_Asteroid,
		Entity_Camera,
		Entity_Pointlight,
		Entity_DirLight,

		Entity_End
	};

	static std::string EntityTypeToString(EntityType type)
	{
		std::string out;

		switch(type )
		{
		case Entity_Empty: out = "empty"; break;
		case Entity_Sky: out = "skybox"; break;
		case Entity_Mesh: out = "mesh"; break;
		case Entity_Asteroid: out = "asteroid"; break;
		case Entity_Camera: out = "camera"; break;
		case Entity_Pointlight: out = "point_light"; break;
		case Entity_DirLight: out = "direction_light"; break;
		}

		return out;
	}

	enum Mesh
	{
		Mesh_Box,
		Mesh_Sphere,
		Mesh_Cylinder,
		Mesh_Cone,
		Mesh_Pyramid,
		Mesh_Asteroid,

		Mesh_End
	};
}