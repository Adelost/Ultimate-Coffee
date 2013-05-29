#include "stdafx.h"
#include "Converter.h"

std::string Converter::IntToStr(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string Converter::FloatToStr(float f)
{
	std::stringstream ss;
	ss << std::fixed;
	ss << f;
	return ss.str();
}

int Converter::StrToInt(std::string str)
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

float Converter::StrToFloat(std::string str)
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

wchar_t* Converter::StrlToWstr(std::string string)
{
	int wchars_num = MultiByteToWideChar(
		CP_UTF8 , 
		0 , 
		string.c_str(), 
		-1, 
		nullptr , 
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

bool Converter::BytesToFile(const char* bytes, int sizeOfBytes, std::string path)
{
	std::ofstream outputFile(path, std::ios::binary);
	if(!outputFile.is_open())
	{
		return false;
	}
	outputFile.write(reinterpret_cast<const char*>(bytes), sizeOfBytes);
	outputFile.close();
	return true;
}

bool Converter::FileToBytes(std::string path, char* bytes, int bufferSize)
{
	std::ifstream inputFile(path, std::ios::binary);
	if(!inputFile.is_open())
	{
		return false;
	}
	inputFile.read(bytes, bufferSize);
	inputFile.close();
	return true;
}