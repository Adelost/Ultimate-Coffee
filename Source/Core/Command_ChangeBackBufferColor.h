#pragma once

#include "Command.h"

class Command_ChangeBackBufferColor :
	public Command
{
private:
	struct BackBufferColorData
	{
		float doX, doY, doZ;
		float undoX, undoY, undoZ;
	};
	BackBufferColorData m_commandData;

protected:
	// virtual overrides from "Command"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&m_commandData);}
	int getByteSizeOfDataStruct(){return sizeof(m_commandData);}

public:
	Command_ChangeBackBufferColor();
	~Command_ChangeBackBufferColor(void);

	// virtual overrides from "Command"
	void doRedo();
	void undo();
	void loadDataStructFromBytes(char* data){m_commandData = *reinterpret_cast<BackBufferColorData*>(data);}

	// Set/get functions
	void setDoColor(float x, float y, float z){m_commandData.doX=x;m_commandData.doY=y;m_commandData.doZ=z;}
	void setUndoColor(float x, float y, float z){m_commandData.undoX=x;m_commandData.undoY=y;m_commandData.undoZ=z;}
	//float getDoColorX(){return commandData_.doX;}
	//float getDoColorY(){return commandData_.doY;}
	//float getDoColorZ(){return commandData_.doY;}
	//float getUndoColorX(){return commandData_.undoX;}
	//float getUndoColorY(){return commandData_.undoY;}
	//float getUndoColorZ(){return commandData_.undoY;}

};