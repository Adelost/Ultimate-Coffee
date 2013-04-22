#include "MainWindow.h"
#include <QApplication>


/**
// If you can't tell what the main() is for
// you should have your programming license revoked
*/
int main(int argc, char *argv[])
{
	// Detect memory leaks
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Create window
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
