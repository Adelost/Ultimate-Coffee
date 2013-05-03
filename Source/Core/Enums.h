#pragma once
namespace Enum
{
	enum CommandType
	{
		TRANSLATE,
		ROTATE,
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
}