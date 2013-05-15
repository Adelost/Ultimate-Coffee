#pragma once

#include <string>

/**
A Utility class to convert between common classes
*/
class Converter
{
public:
	static std::string IntToStr(int n);

	static std::string FloatToStr(float f);

	static int StrToInt(std::string str);

	static float StrToFloat(std::string str);

	static wchar_t* StrlToWstr(std::string string);

	static int convertBetweenCommandHistoryIndexAndGUIListIndex(int commandHistoryIndex, int nrOfCommands);
};