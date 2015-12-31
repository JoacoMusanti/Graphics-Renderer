// Preprocessor directives
#ifdef _DEBUG
#define DEBUG_VS
#define DEBUG_PS
#endif

// Includes 
#include "Globals.h"
// -------------------
#include "CSystem.h"
// -------------------

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

// Start the entry point for every windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	CSystem* system = new CSystem;

	if (!system)		return 0;
	// Initialize our system
	if (!system->InitSystem())		return 0;
	// Start the main loop of the application, all calculations and rendering happens here
	system->RunSystem();
	// Once we are done, call this function to shutdown everything, we free all the memory here
	system->ShutdownSystem();

	delete system;
	system = 0;


	#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
	#endif

	return 0;
}