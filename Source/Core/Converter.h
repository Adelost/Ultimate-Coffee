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
};