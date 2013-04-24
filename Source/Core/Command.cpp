#include "Command.h"
#include <memory> //memcpy

void Command::prependSerializationDataHook(char*& data, int& currentByteIndex)
{
	Enum::CommandType type = getType();
	int byteSizeOfType = sizeof(type);
	memcpy(data+currentByteIndex, &type, byteSizeOfType); //Add the command type to the byte array "data"
	currentByteIndex += byteSizeOfType; //Increment byte array index counter
}