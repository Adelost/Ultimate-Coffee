#pragma once

#include "Command.h"

class Command_ScaleSceneEntity :
	public Command
{
private:
	struct ScaleData
	{
		int idOfScalableSceneEntity;
		float doScaleX, doScaleY, doScaleZ;
		float undoScaleX, undoScaleY, undoScaleZ;
	};
	ScaleData m_commandData;

protected:
	// virtual overrides from "Command"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&m_commandData);}
	int getByteSizeOfDataStruct(){return sizeof(m_commandData);}

public:
	Command_ScaleSceneEntity(int idOfScalableSceneEntity = -1);
	~Command_ScaleSceneEntity(void);

	// virtual overrides from "Command"
	void doRedo();
	void undo();
	void loadDataStructFromBytes(char* data){m_commandData = *reinterpret_cast<ScaleData*>(data);}

	// Set/get functions
	void setDoScale(float x, float y, float z){m_commandData.doScaleX = x; m_commandData.doScaleY = y; m_commandData.doScaleZ = z;}
	void setUndoScale(float x, float y, float z){m_commandData.undoScaleX = x; m_commandData.undoScaleY = y; m_commandData.undoScaleZ = z;}
	float getDoScaleX(){return m_commandData.doScaleX;}
	float getDoScaleY(){return m_commandData.doScaleY;}
	float getDoScaleZ(){return m_commandData.doScaleZ;}
	float getUndoScaleX(){return m_commandData.undoScaleX;}
	float getUndoScaleY(){return m_commandData.undoScaleY;}
	float getUndoScaleZ(){return m_commandData.undoScaleZ;}
};