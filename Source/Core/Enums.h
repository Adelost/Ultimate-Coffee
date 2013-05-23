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
		Entity_Cube,
		Entity_Asteroid,
		Entity_Camera,
		Entity_Pointlight,

		Entity_End
	};
}