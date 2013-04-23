#include "Serializable.h"
#include <memory> //memcpy

void Serializable::receiveDataStructInSerializationFormat(char*& data, int& writeNextByteToThisIndex)
{
	prependSerializationDataHook(data, writeNextByteToThisIndex);

	void* dataStructPointer = accessDerivedClassDataStruct();
	int dataStructByteSize = getByteSizeOfDataStruct();
	memcpy(data+writeNextByteToThisIndex, dataStructPointer, dataStructByteSize); //Copy the struct into the byte array
	writeNextByteToThisIndex += dataStructByteSize; //Increment byte array index counter

	appendSerializationDataHook(data, writeNextByteToThisIndex);
}