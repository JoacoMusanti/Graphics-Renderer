// Includes
// ---------------------------
#include "CSystem.h"
// ---------------------------

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CSystem::CSystem()
{
	// Nullify both pointers
	Input		= NULL;
	Graphics	= NULL;
	Timer		= NULL;
}

CSystem::CSystem(const CSystem& b)
{
	// not used
}

CSystem::~CSystem()
{
	// not used
}

bool CSystem::InitSystem()
{
	// Create 2 ints, the first one will hold the width of our window
	// The second one will hold the height of our window
	int width   = GetSystemMetrics(SM_CXSCREEN);
	int height  = GetSystemMetrics(SM_CYSCREEN);
	// We call InitWindow passing as parameters the width and height of the window, even though they are just 0 right now
	InitWindow(width, height);
	// Initialize the Input object as a member of CInput
	Input = new CInput;
	// If that failed, return false
	if (!Input)
	{
		ShowCursor(true);
		MessageBox(NULL, L"Input object failed to initialize", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	// We then call InitInput, that function will initialize our input object
	Input->InitInput();
	// After the input stuff has been done, we initalize our Graphics object as member of CGraphics
	Graphics = new CGraphics;
	// If that failed, return false
	if (!Graphics)
	{
		ShowCursor(true);
		MessageBox(NULL, L"Graphics object failed to initialize", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	// Call InitGraph and store the return value in result. We pass the window's width, height and handle to that function
	if (!Graphics->InitGraph(width, height, mHwnd))		return false;
	
	return true;
}

void CSystem::InitWindow(int& width, int& height)
{
	// This function is used to create a new window and make it visible using the Windows API
	// First we create a WNDCLASSEX object, which will hold the characteristics of our window
	WNDCLASSEX wnd;
	DEVMODE dmSettings;

	ZeroMemory(&dmSettings, sizeof(dmSettings));

	AppHandler = this;

	#ifdef _DEBUG
		mAppName   = L"engine [DEBUG]";
	#else
		mAppName   = L"engine [RELEASE]";
	#endif

	// Get the value of the instance of the process and save it in mHinstance
	mHinstance = GetModuleHandle(NULL);

	// We begin filling the structure
	wnd.cbClsExtra						= NULL;									// No need for this
	wnd.cbSize							= sizeof(WNDCLASSEX);					// The size of the object equals the size of the struct
	wnd.cbWndExtra						= NULL;									// No need for this
	wnd.hbrBackground					= (HBRUSH)GetStockObject(BLACK_BRUSH);	// We set the background of the window to black
	wnd.hCursor							= LoadCursor(NULL, IDC_ARROW);			// Arrow that will be used in our app
	wnd.hIcon							= LoadIcon(NULL, IDI_WINLOGO);			// Icon of our app (loading default windows logo)
	wnd.hIconSm							= wnd.hIcon;							// Same icon for this
	wnd.hInstance						= mHinstance;							// A value of an HINSTANCE to this process
	wnd.lpfnWndProc						= WndProc;								// Set the long pointer to window procedure as WndProc
	wnd.lpszClassName					= mAppName;								// The class name will have the app name (not important)
	wnd.lpszMenuName					= mAppName;								// The menu of the app will have the app name (not using menues so not important)
	wnd.style							= CS_HREDRAW | CS_VREDRAW;				// Redrawable window

	// We now have to register our window class, so it's recognized by windows
	RegisterClassEx(&wnd);

	width  = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	dmSettings.dmSize			= sizeof(dmSettings);
	dmSettings.dmPelsWidth		= static_cast<unsigned long>(width);
	dmSettings.dmPelsHeight		= static_cast<unsigned long>(height);
	dmSettings.dmBitsPerPel		= 32;
	dmSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);

	// Call CreateWindowsEx and hold the handle in mHwnd
	mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mAppName, mAppName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
	0, 0, width, height, NULL, NULL, mHinstance, NULL);
	
	// Show the windows
	ShowWindow(mHwnd, SW_SHOW);
	// Set the foreground window and focus on this window
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	// Do show cursor
	ShowCursor(true);

	return;
}

void CSystem::ShutdownSystem()
{
	// If Graphics exists
	if (Graphics)
	{
		// Call ShutdownGraph to end all DirectX processes
		Graphics->ShutdownGraph();
		// Release the pointer
		delete Graphics;
		Graphics = 0;
	}
	// If Input exists
	if (Input)
	{
		// Release the pointer
		delete Input;
		Input = 0;
	}
	// Call ShutdownWindow to kill our window and free remaining memory
	ShutdownWindow();

	#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
	#endif

	return;
}

void CSystem::RunSystem()
{
	bool done = false;
	// Create a new MSG struct object
	MSG msg;

	// Fill the struct with zeroes
	ZeroMemory(&msg, sizeof(MSG));

	Timer = new CTimer;

	uSlot = 0;

	while (!done)
	{
		// Check if there is a message
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// If there is, translate it 
			TranslateMessage(&msg);
			// And then send it to WinProc
			DispatchMessage(&msg);
		}
		// If the user wants to kill the app
		if (msg.message == WM_QUIT)
		{
			// Finish the loop
			done = true;
		}
		// if the user does not want to kill the app
		else
		{
			// Render a frame
			if (!Frame())
			{
				done = true;
			}
		}
	}

	delete Timer;
	Timer = 0;

	return;
}

bool CSystem::Frame()
{
	// If ESC key is being pressed
	if (Input->IsKeyDown(static_cast<unsigned short>(VK_ESCAPE)))
	{
		PostQuitMessage(0);
		return 0;
	}
	if (Input->IsKeyDown(VK_SPACE))
	{
		if(Input->IsKeyDown(VK_UP))
		{
			Graphics->GetModel(uSlot)->RotateModelX(0.01f);
		}
		if(Input->IsKeyDown(VK_DOWN))
		{
			Graphics->GetModel(uSlot)->RotateModelX(-0.01f);
		}
		if(Input->IsKeyDown(VK_LEFT))
		{
			Graphics->GetModel(uSlot)->RotateModelY(0.01f);
		}
		if(Input->IsKeyDown(VK_RIGHT))
		{
			Graphics->GetModel(uSlot)->RotateModelY(-0.01f);
		}
		if(Input->IsKeyDown(VK_SHIFT))
		{
			Graphics->GetModel(uSlot)->RotateModelZ(0.01f);
		}
		if(Input->IsKeyDown(VK_CONTROL))
		{
			Graphics->GetModel(uSlot)->RotateModelZ(-0.01f);
		}
	}
	if (Input->IsKeyDown(0x57))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(0.0f, 0.0f, 0.01f);
	}
	if (Input->IsKeyDown(0x53))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(0.0f, 0.0f, -0.01f);
	}
	if (Input->IsKeyDown(0x44))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(0.01f, 0.0f, 0.0f);
	}
	if (Input->IsKeyDown(0x41))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(-0.01f, 0.0f, 0.0f);
	}
	if (Input->IsKeyDown(0x51))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(0.0f, 0.01f, 0.0f);
	}
	if (Input->IsKeyDown(0x45))
	{
		Graphics->GetModel(uSlot)->TranslateModelIncremental(0.0f, -0.01f, 0.0f);
	}
	if (Input->IsKeyDown(VK_RETURN))
	{
		Graphics->GetModel(uSlot)->TranslateModelTo(0.0f, 0.0f, 0.0f);
		Input->KeyUp(VK_RETURN);
	}
	if (Input->IsKeyDown(VK_ADD))
	{
		uSlot++;
		Input->KeyUp(VK_ADD);
	}
	if (Input->IsKeyDown(VK_SUBTRACT))
	{
		uSlot--;
		Input->KeyUp(VK_SUBTRACT);
	}
	if (Input->IsKeyDown(VK_TAB))
	{
		if (::gLoadingScreen)
		{
			::gLoadingScreen = false;
			Graphics->TurnZOn();
		}
		else
		{
			::gLoadingScreen = true;
			Graphics->TurnZOff();
		}
		Input->KeyUp(VK_TAB);
	}
	
	if (!Graphics->Render())		return false;
	
	return true;
}

LRESULT CALLBACK CSystem::MessageHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Switch through every available message
	switch(msg)
	{
		// If a key is being pressed
	case WM_KEYDOWN:
		{
			// Call KeyDown and set it as pressed
			Input->KeyDown(wparam);
			return 0;
			break;
		}
		// If a key is released
	case WM_KEYUP:
		{
			// Call KeyUp and set it as released
			Input->KeyUp(wparam);
			return 0;
			break;
		}
	default:
		{
			// Else, send the message to the default windows procedure
			return DefWindowProc(hWnd, msg, wparam, lparam);
			break;
		}
	}
	return 0;
}

void CSystem::ShutdownWindow()
{
	// Show the cursor
	ShowCursor(true);

	// If we were running ::gFullScreen, disable it
	if (::gFullScreen)
	{
		ChangeDisplaySettings(0, 0);
	}

	// Destroy our window and release the handle
	DestroyWindow(mHwnd);
	mHwnd = 0;

	// Unregister our class and release the instance
	UnregisterClass(mAppName, mHinstance);
	mHinstance = 0;

	// Release the pointer to the window
	AppHandler = 0;
	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Switch through every available message and check if
	switch (msg)
	{	
	case WM_CREATE:
		{
			break;
		}
	case WM_ACTIVATE:
        switch (wparam)
        {
        default:
            break;

        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            break;

        case WA_INACTIVE:
            if (::gFullScreen)
                ShowWindow(hWnd, SW_MINIMIZE);
            break;
        }
        break;
		// The user wants to close the window
	case WM_CLOSE:
		{
			// exit app
			PostQuitMessage(0);
			return 0;
		}
		// Send every other message to our MessageHandler function
	default:
		{
			return AppHandler->MessageHandler(hWnd, msg, wparam, lparam);
		}
	}
	return 0;
}

