#pragma once

#include "Command.h"

class Command_TranslateSceneEntity :
	public Command
{
private:
	struct TranslationData
	{
		int idOfTranslatableSceneEntity;
		float doTranslationX, doTranslationY, doTranslationZ;
		float undoTranslationX, undoTranslationY, undoTranslationZ;
	};
	TranslationData m_commandData;

protected:
	// virtual overrides from "Command"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&m_commandData);}
	int getByteSizeOfDataStruct(){return sizeof(m_commandData);}

public:
	Command_TranslateSceneEntity(int idOfTranslatableSceneEntity = -1);
	~Command_TranslateSceneEntity(void);

	// virtual overrides from "Command"
	void doRedo();
	void undo();
	void loadDataStructFromBytes(char* data){m_commandData = *reinterpret_cast<TranslationData*>(data);}

	// Set/get functions
	void setDoTranslation(float x, float y, float z){m_commandData.doTranslationX = x; m_commandData.doTranslationY = y; m_commandData.doTranslationZ = z;}
	void setUndoTranslation(float x, float y, float z){m_commandData.undoTranslationX = x; m_commandData.undoTranslationY = y; m_commandData.undoTranslationZ = z;}
	float getDoTranslationX(){return m_commandData.doTranslationX;}
	float getDoTranslationY(){return m_commandData.doTranslationY;}
	float getDoTranslationZ(){return m_commandData.doTranslationZ;}
	float getUndoTranslationX(){return m_commandData.undoTranslationX;}
	float getUndoTranslationY(){return m_commandData.undoTranslationY;}
	float getUndoTranslationZ(){return m_commandData.undoTranslationZ;}

};