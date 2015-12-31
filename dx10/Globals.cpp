#include "Globals.h"

bool ::gLoadingScreen	= false;
bool ::gFullScreen		= false;

bool Error(LPWSTR error)
{
	MessageBox(0, error, L"FATAL ERROR", MB_OK | MB_ICONERROR);

	return false;
}

void StrToLPWSTR(string ConvertMe, LPWSTR &_Out)
{
	char *ANSIstr = new char[ConvertMe.length() + 1];

	for (unsigned int f = 0; f < ConvertMe.length(); f++)
	{
		ANSIstr[f] = ConvertMe[f];
	}

	ANSIstr[ConvertMe.length()] = '\0';

	int strLNG = lstrlenA(ANSIstr);

	LPWSTR UNICODEstr = SysAllocStringLen(NULL, strLNG);

	::MultiByteToWideChar(CP_ACP, 0, ANSIstr, strLNG, UNICODEstr, strLNG);

	int lenA = lstrlenA(ANSIstr);
	int lenW;
	lenW = ::MultiByteToWideChar(CP_ACP, 0, ANSIstr, lenA, 0, 0);

	UNICODEstr = ::SysAllocStringLen(0, lenW);
	::MultiByteToWideChar(CP_ACP, 0, ANSIstr, lenA, UNICODEstr, lenW);

	delete[] ANSIstr;
	ANSIstr = 0;

	_Out = UNICODEstr;

	::SysFreeString(UNICODEstr);

	return ;
}