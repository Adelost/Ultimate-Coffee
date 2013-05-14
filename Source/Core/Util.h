#pragma once

#include <iostream>
#include <sstream>

// Prints the supplied string to console
#define DEBUGPRINT(DATA_STREAM)								\
{															\
	std::ostringstream oss;									\
	oss << DATA_STREAM << std::endl;						\
	printf(oss.str().c_str());								\
}

// Prints string to the "Output" window of Visual Studio using OutputDebugString
#define OUTPUT_WINDOW_PRINT(stream)				\
{												\
	std::wostringstream os;						\
	os << stream << "\n";						\
	OutputDebugString(os.str().c_str());		\
}