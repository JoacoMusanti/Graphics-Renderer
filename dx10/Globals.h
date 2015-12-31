#ifndef GLOBALS_H_
#define GLOBALS_H_

// Includes
#include <crtdbg.h>
#include <stdint.h>
#include <cstdlib>
#include <Windows.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <xnamath.h>
#include <D3DX11.h>
#include <string>
#include <vector>
#include <fstream>

// Links
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dx11")
#pragma comment(lib, "d3dx10")

using namespace std;

extern bool gLoadingScreen;
extern bool gFullScreen;
static bool gVSync			  = true;

static float gCamNear	      = 0.1f;
static float gCamDepth        = 1000.0f;

const unsigned int RENDER_MODE_SPRITE = 100;
const unsigned int RENDER_MODE_MODEL  = 101;


void StrToLPWSTR(string ConvertMe, LPWSTR &_Out);

bool Error(LPWSTR error);

#endif