#pragma once

#include <memory>
#include <Core/Data.h>
#include <Core/DataMapper.h>
#include "Core/ISystem.h"
#include "Core/Events.h"
#include "OculusManager.h"


class DXRenderer;
class Manager_3DTools;

namespace System
{
	/**
	Beginning of Render System implementation.
	NOTE: Not done yet.
	*/
	class Render : public Type<Render>
	{
	private:
		DataMapper<Data::Transform> m_mapper_position;
		DXRenderer* renderer;
		OculusManager* m_oculus;

	public:
		Render();
		~Render();
		void setupDirectX();
		void update();
	};
}