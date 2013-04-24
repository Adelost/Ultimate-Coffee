#pragma once
//--------------------------------------------------------------------------------------
// Functionality:
// * Outputs a struct containing plain old data (no pointers) as a byte array (char array): "receiveDataStructInSerializationFormat".
// * Loads a struct from byte array (char array): "loadDataStructFromBytes".
//
// For the class to work as intended, "accessDerivedClassDataStruct", "getByteSizeOfDataStruct",
// and "loadDataStructFromBytes" needs to overloaded correctly. Refer to their respective comment below.
//--------------------------------------------------------------------------------------
class Serializable
{
public:
	Serializable(void){};
	virtual ~Serializable(void){};

	virtual void prependSerializationDataHook(char*& data, int& writeNextByteToThisIndex){} //Hook in before "receiveDataStructInSerializationFormat"
	void receiveDataStructInSerializationFormat(char*& data, int& writeNextByteToThisIndex);//Template function. The data struct is stored in the byte array "data" at "currentByteIndex"
	virtual void appendSerializationDataHook(char*& data, int& currentByteIndex){}			//Hook in after "receiveDataStructInSerializationFormat"

	//Called in "receiveDataStructInSerializationFormat"
	virtual void* accessDerivedClassDataStruct() = 0;		//What derived class should do:	reinterpret_cast<void*>(&dataStruct_);
	virtual int getByteSizeOfDataStruct() = 0;				//What derived class should do:	sizeof(dataStruct_);

	virtual void loadDataStructFromBytes(char* data) = 0;	//What derived class should do:	dataStruct_ = *reinterpret_cast<DataStruct*>(&dataStruct);
};