#pragma once

#include "Command.h"
#include "Math.h"

class Command_SkyBox : public Command
{
public:
	struct dataStruct
	{
		int skyBoxIndex;
		int undoSkyBoxIndex;
		Vector3 doColor;
		Vector3 undoColor;
	};

	dataStruct dataStruct_;

public:
	Command_SkyBox();
	~Command_SkyBox();

	virtual void doRedo();
	virtual void undo();

	virtual void* accessDerivedClassDataStruct();
	virtual int getByteSizeOfDataStruct();

	virtual void loadDataStructFromBytes(char* data);

	int getSkyBoxIndex(){return dataStruct_.skyBoxIndex;}
};