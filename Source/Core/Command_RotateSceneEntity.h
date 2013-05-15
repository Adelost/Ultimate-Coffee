#pragma once

#include "Command.h"

class Command_RotateSceneEntity :
	public Command
{
private:
	struct RotationData
	{
		int idOfTranslatableSceneEntity;
		float doRotQuatX, doRotQuatY, doRotQuatZ, doRotQuatW;
		float undoRotQuatX, undoRotQuatY, undoRotQuatZ, undoRotQuatW;
	};
	RotationData m_commandData;

protected:
	// virtual overrides from "Command"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&m_commandData);}
	int getByteSizeOfDataStruct(){return sizeof(m_commandData);}

public:
	Command_RotateSceneEntity(int idOfRotatableSceneEntity = -1);
	~Command_RotateSceneEntity(void);

	// virtual overrides from "Command"
	void doRedo();
	void undo();
	void loadDataStructFromBytes(char* data){m_commandData = *reinterpret_cast<RotationData*>(data);}

	// Set/get functions
	void setDoRotQuat(float x, float y, float z, float w){m_commandData.doRotQuatX = x; m_commandData.doRotQuatY = y; m_commandData.doRotQuatZ = z; m_commandData.doRotQuatW = w;}
	void setUndoRotQuat(float x, float y, float z, float w){m_commandData.undoRotQuatX = x; m_commandData.undoRotQuatY = y; m_commandData.undoRotQuatZ = z; m_commandData.undoRotQuatW = w;}
	float getDoRotQuatX(){return m_commandData.doRotQuatX;}
	float getDoRotQuatY(){return m_commandData.doRotQuatY;}
	float getDoRotQuatZ(){return m_commandData.doRotQuatZ;}
	float getDoRotQuatW(){return m_commandData.doRotQuatW;}
	float getUndoRotQuatX(){return m_commandData.undoRotQuatX;}
	float getUndoRotQuatY(){return m_commandData.undoRotQuatY;}
	float getUndoRotQuatZ(){return m_commandData.undoRotQuatZ;}
	float getUndoRotQuatW(){return m_commandData.undoRotQuatW;}
};