#pragma once

#include "Enums.h"
#include "Serializable.h"
//--------------------------------------------------------------------------------------
// Encapsulates doings and undoings of an operation,
// by storing the data needed when doing the operation
// and the data needed when undoing the operation, in a struct.
//
// The doings and undoing are deferred to derived classes overloading "doRedo" and "undo".
//
// A command is serializable if its plain old data is stored in a struct, and if all
// pure virtual overloads from "Serializable" are overloaded correctly (refer to "Serializable.h").
//--------------------------------------------------------------------------------------
class Command : 
	public Serializable
{
private:
	Enum::CommandType m_type;

protected:
	void setType(Enum::CommandType type){m_type = type;}

public:
	Command(void){m_type=Enum::CommandType::TRANSLATE;} // Default command: Enum::CommandType::TRANSLATE. May be changed by derived classes by calling "setType"
	virtual ~Command(void){};
	
	Enum::CommandType getType(){return m_type;}

	// virtual overload from "Serializable"
	void prependSerializationDataHook(char*& data, int& currentByteIndex); // Add the command type before the struct data

	virtual void doRedo() = 0;
	virtual void undo() = 0;
};