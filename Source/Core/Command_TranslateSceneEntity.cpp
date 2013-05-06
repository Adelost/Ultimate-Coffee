#include "stdafx.h"
#include "Command_TranslateSceneEntity.h"
#include "Events.h"

Command_TranslateSceneEntity::Command_TranslateSceneEntity(int idOfTranslatableSceneEntity)
{
	m_commandData.idOfTranslatableSceneEntity = idOfTranslatableSceneEntity;

	m_commandData.doTranslationX=0.0f;
	m_commandData.doTranslationY=0.0f;
	m_commandData.doTranslationZ=0.0f;

	m_commandData.undoTranslationX=0.0f;
	m_commandData.undoTranslationY=0.0f;
	m_commandData.undoTranslationZ=0.0f;

	setType(Enum::CommandType::TRANSLATE_SCENE_ENTITY);
}

Command_TranslateSceneEntity::~Command_TranslateSceneEntity(void)
{
}

void Command_TranslateSceneEntity::doRedo()
{
	SEND_EVENT(&Event_TranslateSceneEntity(m_commandData.idOfTranslatableSceneEntity, m_commandData.doTranslationX, m_commandData.doTranslationY, m_commandData.doTranslationZ));
}

void Command_TranslateSceneEntity::undo()
{
	SEND_EVENT(&Event_TranslateSceneEntity(m_commandData.idOfTranslatableSceneEntity, m_commandData.undoTranslationX, m_commandData.undoTranslationY, m_commandData.undoTranslationZ));
}