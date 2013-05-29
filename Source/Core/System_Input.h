#pragma once

#include "ISystem.h"

namespace System
{
	class Input : public Type<Input>
	{
	public:
		void update();

	private:
		void updateZoomTo();
	};

}






