#ifndef CINPUT_H_
#define CINPUT_H_

class CInput
{
public:
	// Constructors for this class
	CInput();
	CInput(const CInput&);
	~CInput();
	// Public members of this class
	void InitInput();
	bool IsKeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	void KeyDown(unsigned int key);

private:
	// Private members of this class
	bool Keys[256];
};

#endif