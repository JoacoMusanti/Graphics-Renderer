// Includes
// --------------------
#include "CInput.h"
// --------------------

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CInput::CInput()
{
	// not used
}

CInput::CInput(const CInput& any)
{
	// not used
}

CInput::~CInput()
{
	// not used
}

void CInput::InitInput()
{
	// Initialize all this bools to false, meaning that there are no keys pressed when the program starts
	for (unsigned int i = 0; i < 256; i++)
	{
		Keys[i] = false;
	}
}

bool CInput::IsKeyDown(unsigned int key)
{
	// Returns the boolean value of the key we want to know about, returns true == key is being pressed
	// Returns false == key is not being pressed
	return Keys[key];
}

void CInput::KeyUp(unsigned int key)
{
	// If a key has been released, we just fill that key value with a false
	Keys[key] = false;
	return;
}

void CInput::KeyDown(unsigned int key)
{
	// If a key has been pressed, we fill the key value with a true
	Keys[key] = true;
	return;
}