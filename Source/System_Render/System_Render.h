#pragma once

#include <Core/World.h>
#include <Core/Events.h>


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

	public:
		Render();
		~Render();
		void setupDirectX();
		void update();
		void process();
	};
}