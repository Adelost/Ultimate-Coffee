#pragma once

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/ISystem.h>

class DXRenderer;

namespace System
{
	/**
	Beginning of Render System implementation.
	NOTE: Not done yet.
	*/

	class Render : public Type<Render>
	{
	private:
		DataMapper<Data::Position> m_mapper_position;
		DXRenderer* renderer;

	public:
		Render();
		~Render();
		void setupDirectX();
		void update();
		void process();
	};
}