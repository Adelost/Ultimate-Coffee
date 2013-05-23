#pragma once

#include <string>
#include <iomanip>
#include <sstream>

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

	static int convertFromCommandHistoryIndexToCommandHistoryGUIListIndex(int commandHistoryIndex);
	static int convertFromCommandHistoryGUIListIndexToCommandHistoryIndex(int commandHistoryGUIListIndex);
};