#pragma once

#include "IData.h"

namespace Data
{
	class Position : public Type<Position>
	{
	public:
		float x;
		float y;
	};
}

