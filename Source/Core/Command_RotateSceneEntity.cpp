#include "stdafx.h"
#include "Command_RotateSceneEntity.h"
#include "Events.h"

Command_RotateSceneEntity::Command_RotateSceneEntity(int idOfTranslatableSceneEntity)
{
	m_commandData.idOfTranslatableSceneEntity = idOfTranslatableSceneEntity;

	m_commandData.doRotQuatX=0.0f;
	m_commandData.doRotQuatY=0.0f;
	m_commandData.doRotQuatZ=0.0f;
	m_commandData.doRotQuatW=0.0f;

	m_commandData.undoRotQuatX=0.0f;
	m_commandData.undoRotQuatY=0.0f;
	m_commandData.undoRotQuatZ=0.0f;
	m_commandData.undoRotQuatW=0.0f;

	setType(Enum::CommandType::ROTATE_SCENE_ENTITY);
}

Command_RotateSceneEntity::~Command_RotateSceneEntity(void)
{
}

void Command_RotateSceneEntity::doRedo()
{
	SEND_EVENT(&Event_RotateSceneEntity(m_commandData.idOfTranslatableSceneEntity, m_commandData.doRotQuatX, m_commandData.doRotQuatY, m_commandData.doRotQuatZ, m_commandData.doRotQuatW));
}

void Command_RotateSceneEntity::undo()
{
	SEND_EVENT(&Event_RotateSceneEntity(m_commandData.idOfTranslatableSceneEntity, m_commandData.undoRotQuatX, m_commandData.undoRotQuatY, m_commandData.undoRotQuatZ, m_commandData.undoRotQuatW));
}