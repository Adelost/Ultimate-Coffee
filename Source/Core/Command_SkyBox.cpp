#include "stdafx.h"
#include "Command_SkyBox.h"

#include "World.h"

Command_SkyBox::Command_SkyBox()
{
	setType(Enum::CommandType::SKYBOX);
}

Command_SkyBox::~Command_SkyBox()
{

}

void Command_SkyBox::setShowSkyBox(bool showSkyBox)
{
	dataStruct_.showSkyBox = showSkyBox;
}

void Command_SkyBox::doRedo()
{
	SETTINGS()->showSkybox = dataStruct_.showSkyBox;
}

void Command_SkyBox::undo()
{
	SETTINGS()->showSkybox = !dataStruct_.showSkyBox;
}

void* Command_SkyBox::accessDerivedClassDataStruct()
{
	return reinterpret_cast<void*>(&dataStruct_);
}

int Command_SkyBox::getByteSizeOfDataStruct()
{
	return sizeof(dataStruct_);
}

void Command_SkyBox::loadDataStructFromBytes(char* data)
{
	dataStruct_ = *reinterpret_cast<dataStruct*>(&data);
}