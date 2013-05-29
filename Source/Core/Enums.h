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
		Entity_Camera,
		Entity_Sky,
		Entity_DirLight,
		Entity_Pointlight,
		Entity_Mesh,
		Entity_Asteroid,
	
		Entity_End
	};

	static std::string EntityTypeToString(EntityType type)
	{
		std::string out;

		switch(type )
		{
		case Entity_Empty:		out = "Empty";				break;
		case Entity_Sky:		out = "Skybox";				break;
		case Entity_Mesh:		out = "Mesh";				break;
		case Entity_Asteroid:	out = "Asteroid";			break;
		case Entity_Camera:		out = "Camera";				break;
		case Entity_Pointlight: out = "PointLight";		break;
		case Entity_DirLight:	out = "DirectionLight";	break;
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
		Mesh_Sphere_LowPoly,
		Mesh_Al,

		Mesh_End
	};
}