#pragma once

#include "Command.h"



class Command_SkyBox : public Command
{
	struct dataStruct
	{
		bool showSkyBox;
	};

	dataStruct dataStruct_;

public:
	Command_SkyBox();
	~Command_SkyBox();

	void setShowSkyBox(bool showSkyBox);

	virtual void doRedo();
	virtual void undo();

	virtual void* accessDerivedClassDataStruct();
	virtual int getByteSizeOfDataStruct();

	virtual void loadDataStructFromBytes(char* data);
};