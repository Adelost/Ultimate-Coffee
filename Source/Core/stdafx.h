#pragma once

#include <bitset>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#include "Math.h"
#include "Settings.h"

#include "IData.h"
#include "ISystem.h"
#include "Manager_Data.h"

#include "Entity.h"


// Prints the supplied string to console
#define DEBUGPRINT(DATA_STREAM)									\
	{															\
		std::ostringstream oss;									\
		oss << DATA_STREAM << std::endl;						\
		printf(oss.str().c_str());								\
	}