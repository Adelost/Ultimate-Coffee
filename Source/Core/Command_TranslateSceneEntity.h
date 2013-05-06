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
	float getDoColorX(){return m_commandData.doTranslationX;}
	float getDoColorY(){return m_commandData.doTranslationY;}
	float getDoColorZ(){return m_commandData.doTranslationZ;}
	float getUndoColorX(){return m_commandData.undoTranslationX;}
	float getUndoColorY(){return m_commandData.undoTranslationY;}
	float getUndoColorZ(){return m_commandData.undoTranslationZ;}

};