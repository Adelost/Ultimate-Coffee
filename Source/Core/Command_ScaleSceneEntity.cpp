#include "stdafx.h"
#include "Command_ScaleSceneEntity.h"
#include "Events.h"

Command_ScaleSceneEntity::Command_ScaleSceneEntity(int idOfTranslatableSceneEntity)
{
	m_commandData.idOfScalableSceneEntity = idOfTranslatableSceneEntity;

	m_commandData.doScaleX=0.0f;
	m_commandData.doScaleY=0.0f;
	m_commandData.doScaleZ=0.0f;

	m_commandData.undoScaleX=0.0f;
	m_commandData.undoScaleY=0.0f;
	m_commandData.undoScaleZ=0.0f;

	setType(Enum::CommandType::TRANSLATE_SCENE_ENTITY);
}

Command_ScaleSceneEntity::~Command_ScaleSceneEntity(void)
{
}

void Command_ScaleSceneEntity::doRedo()
{
	SEND_EVENT(&Event_ScaleSceneEntity(m_commandData.idOfScalableSceneEntity, m_commandData.doScaleX, m_commandData.doScaleY, m_commandData.doScaleZ));
}

void Command_ScaleSceneEntity::undo()
{
	SEND_EVENT(&Event_ScaleSceneEntity(m_commandData.idOfScalableSceneEntity, m_commandData.undoScaleX, m_commandData.undoScaleY, m_commandData.undoScaleZ));
}