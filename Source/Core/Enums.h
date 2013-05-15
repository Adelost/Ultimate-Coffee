#pragma once
namespace Enum
{
	enum CommandType
	{
		TRANSLATE_SCENE_ENTITY,
		ROTATE_SCENE_ENTITY,
		SCALE_SCENE_ENTITY,
		CHANGEBACKBUFFERCOLOR
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
}