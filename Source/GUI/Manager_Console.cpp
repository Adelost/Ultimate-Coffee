#include "stdafx.h"
#include "Manager_Console.h"

#include <Core/EventManager.h>

// Stuff used to allocate console
// no idea what most of it does
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

void Manager_Console::init()
{
	// Create debug console
	AllocConsole();
	SetConsoleTitle(L"Debug console");
	int hConHandle;
	long lStdHandle;
	FILE *fp;   // redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, nullptr, _IONBF, 0 );

	// Test print
	DEBUGPRINT("////////////////////");
	DEBUGPRINT("  								");
	DEBUGPRINT("  INITIALIZING					");
	DEBUGPRINT("  Debug Console					");
	DEBUGPRINT("  								");
	DEBUGPRINT("    ~-_	  Steamin'				");
	DEBUGPRINT("     _-~    Hot					");
	DEBUGPRINT("   c|_|	  COFFEE				");
	DEBUGPRINT("  								");
	DEBUGPRINT("////////////////////");
	DEBUGPRINT("  								");
}