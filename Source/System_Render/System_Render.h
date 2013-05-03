#pragma once

#include <Core/Data.h>
#include <Core/DataMapper.h>
#include "Core/ISystem.h"
#include "Core/Events.h"
#include "ITool_Transformation.h"
#include "Tool_Translation.h"

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
		DataMapper<Data::Transform> m_mapper_position;
		DXRenderer* renderer;
		ITool_Transformation* currentlyChosenTransformTool;
		Tool_Translation* theTranslationTool;

	public:
		Render();
		~Render();
		void onEvent(IEvent* p_event);
		void setupDirectX();
		void update();
		void process();
	};
}