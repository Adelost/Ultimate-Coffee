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

	static int ConvertFromCommandHistoryIndexToCommandHistoryGUIListIndex(int commandHistoryIndex);
	static int ConvertFromCommandHistoryGUIListIndexToCommandHistoryIndex(int commandHistoryGUIListIndex);

	static bool BytesToFile(const char* bytes, int sizeOfBytes, std::string path);
	static bool FileToBytes(std::string path, char* bytes, int bufferSize);
};