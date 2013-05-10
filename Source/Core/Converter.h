#pragma once

#include <string>
#include <sstream>
#include <iomanip>


/**
A Utility class to convert between common classes
*/
class Converter
{
public:
	static std::string IntToStr(int n)
	{
		std::stringstream ss;
		ss << n;
		return ss.str();
	}

	static std::string FloatToStr(float f)
	{
		std::stringstream ss;
		ss << std::fixed;
		ss << f;
		return ss.str();
	}

	static int StrToInt(std::string str)
	{
		int n;
		std::stringstream ss(str);
		ss >> n;
		if(ss.fail())
		{
			n = -1;
		}

		return n;
	}

	static float StrToFloat(std::string str)
	{
		float n;
		std::stringstream ss(str);
		ss >> n;
		if(ss.fail())
		{
			n = -1.0f;
		}

		return n;
	}

	static wchar_t* StrlToWstr(std::string string)
	{
		int wchars_num = MultiByteToWideChar(
			CP_UTF8 , 
			0 , 
			string.c_str(), 
			-1, 
			NULL , 
			0);
		wchar_t* wstr = new wchar_t[wchars_num];

		MultiByteToWideChar(
			CP_UTF8, 
			0,
			string.c_str(),
			-1, 
			wstr , 
			wchars_num);

		return wstr;
	}

	static int convertBetweenCommandHistoryIndexAndGUIListIndex(int commandHistoryIndex, int nrOfCommands)
	{
		/*
		The index of a command in the command history and the corresponding command in the GUI list is not the same,
		because of "commandHistoryListWidget->insertItem(0, item);" in "Manager_Docks.cpp"
	
		Below example format: index, command number
		----------------------------------------------------
		->Command history
		0, command 1
		1, command 2
		2, command 3
		3, command 4
	
		->GUI
		0, command 4
		1, command 3
		2, command 2
		3, command 1
		----------------------------------------------------
		->Mapping:
		f(0)=3
		f(1)=2
		f(2)=1
		f(3)=0

		nrOfCommands: 4
		f(0)=4-1-0=3
		f(1)=4-1-1=2
		f(2)=4-1-2=1
		f(3)=4-1-3=0
		----------------------------------------------------
		*/
		if(commandHistoryIndex != -1)
		{
			return nrOfCommands-1-commandHistoryIndex;
		}
		else //special case: jump out of history
		{
			return -1;
		}
	}
};