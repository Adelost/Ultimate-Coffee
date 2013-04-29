#ifndef IHANDLE_H
#define IHANDLE_H

#include <Windows.h>
//#include <xnamath.h>

class IHandle
{
private:
public:
	virtual bool getIsSelected() = 0;
};

#endif