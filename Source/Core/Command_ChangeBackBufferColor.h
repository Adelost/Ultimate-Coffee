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
	BackBufferColorData commandData_;

protected:
	//virtual overrides from "Command"
	void* accessDerivedClassDataStruct(){return reinterpret_cast<void*>(&commandData_);}
	int getByteSizeOfDataStruct(){return sizeof(commandData_);}

public:
	Command_ChangeBackBufferColor();
	~Command_ChangeBackBufferColor(void);

	//virtual overrides from "Command"
	void doRedo();
	void undo();
	void loadDataStructFromBytes(char* data){commandData_ = *reinterpret_cast<BackBufferColorData*>(data);}

	//Set/get functions
	void setDoColor(float x, float y, float z){commandData_.doX=x;commandData_.doY=y;commandData_.doZ=z;}
	void setUndoColor(float x, float y, float z){commandData_.undoX=x;commandData_.undoY=y;commandData_.undoZ=z;}
	float getDoColorX(){return commandData_.doX;}
	float getDoColorY(){return commandData_.doY;}
	float getDoColorZ(){return commandData_.doY;}
	float getUndoColorX(){return commandData_.undoX;}
	float getUndoColorY(){return commandData_.undoY;}
	float getUndoColorZ(){return commandData_.undoY;}

};