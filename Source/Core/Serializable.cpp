#include "stdafx.h"
#include "Serializable.h"
#include <memory> // memcpy

void Serializable::receiveDataStructInSerializationFormat( char*& p_data, int& p_writeNextByteToThisIndex )
{
	prependSerializationDataHook(p_data, p_writeNextByteToThisIndex);

	void* dataStructPointer = accessDerivedClassDataStruct();
	int dataStructByteSize = getByteSizeOfDataStruct();
	memcpy(p_data+p_writeNextByteToThisIndex, dataStructPointer, dataStructByteSize); // Copy the struct into the byte array
	p_writeNextByteToThisIndex += dataStructByteSize; // Increment byte array index counter

	appendSerializationDataHook(p_data, p_writeNextByteToThisIndex);
}