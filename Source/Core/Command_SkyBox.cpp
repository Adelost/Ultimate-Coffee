#include "stdafx.h"
#include "Command_SkyBox.h"

#include "World.h"
#include "Events.h"

Command_SkyBox::Command_SkyBox()
{
	setType(Enum::CommandType::SKYBOX);
	dataStruct_.skyBoxIndex = 0;
	dataStruct_.undoSkyBoxIndex = 0;
}

Command_SkyBox::~Command_SkyBox()
{

}


void Command_SkyBox::doRedo()
{
	dataStruct_.undoSkyBoxIndex = SETTINGS()->skyboxIndex();
	SETTINGS()->setSkyboxIndex(dataStruct_.skyBoxIndex);
	SEND_EVENT(&Event_SetBackBufferColor(dataStruct_.doColor.x, dataStruct_.doColor.y, dataStruct_.doColor.z));
}

void Command_SkyBox::undo()
{
	SETTINGS()->setSkyboxIndex(dataStruct_.undoSkyBoxIndex);
	SEND_EVENT(&Event_SetBackBufferColor(dataStruct_.undoColor.x, dataStruct_.undoColor.y, dataStruct_.undoColor.z));
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
	dataStruct_ = *reinterpret_cast<dataStruct*>(data);
}