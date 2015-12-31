#ifndef CSYSTEM_H_
#define CSYSTEM_H_

// Includes
// ---------------------------------
#include "CGraphics.h"
#include "CInput.h"
// ---------------------------------

class CSystem
{
public:
	// Constructors and destructors for this class
	CSystem();
	CSystem(const CSystem&);
	~CSystem();
	// Public members of this class
	bool InitSystem();
	void ShutdownSystem();
	void RunSystem();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	// Private members of this class
	bool Frame();
	void InitWindow(int&, int&);
	void ShutdownWindow();

	HINSTANCE mHinstance;
	HWND	  mHwnd;
	LPCWSTR   mAppName;

	CInput	  *Input;
	CGraphics *Graphics;
	CTimer    *Timer;

	unsigned int uSlot;
};

// Prototypes
// ------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// ------------------------------------------------------------

// Globals
// --------------------------------------------------------
static CSystem		*AppHandler = NULL;
// --------------------------------------------------------

#endif