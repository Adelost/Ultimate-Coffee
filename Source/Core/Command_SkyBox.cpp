#include "stdafx.h"
#include "Command_SkyBox.h"

#include "World.h"
#include "Events.h"

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
	SETTINGS()->setShowSkybox(dataStruct_.showSkyBox);
	SEND_EVENT(&Event_SetBackBufferColor(dataStruct_.doColor.x, dataStruct_.doColor.y, dataStruct_.doColor.z));
}

void Command_SkyBox::undo()
{
	SETTINGS()->setShowSkybox(!dataStruct_.showSkyBox);
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