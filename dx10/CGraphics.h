#ifndef CGRAPHICS_H_
#define CGRAPHICS_H_

// Includes
// ----------------------------
#include "Globals.h"
#include "CModel.h"
#include "CCamera.h"
#include "CImporter.h"
#include "CShader.h"
#include "Light.h"
#include "CSprites.h"
// ----------------------------

// Namespace
// ------------------------------------
using namespace std;
// ------------------------------------

class CGraphics
{
public:
	// Constructors and destructors for this class
	CGraphics();
	CGraphics(const CGraphics&);
	~CGraphics();
	// Public members of this class
	bool Render();
	bool InitGraph(int&, int&, HWND);
	void ShutdownGraph();
	bool ResizeBuffers(HWND hMainWindow);

	
	
	
	// Inline functions definition

	inline CCamera	 *GetCamera()
	{	
		return camera;
	}

	inline void RotatesCameraIncrement(float x, float y, float z)
	{
		camera->RotateCameraIncrement(x, y, z);
		return;
	}
	inline void MovesCameraIncrement(float x, float y, float z)
	{
		camera->MoveCameraIncrement(x, y, z);
		return;
	}
	inline void TurnZOn()
	{
		this->pDeviceContext->OMSetDepthStencilState(pDepthStencilState, 1);
		return;
	}
	inline void TurnZOff()
	{
		this->pDeviceContext->OMSetDepthStencilState(pDepthStencilStateDisabled, 1);
		return;
	}

	inline CModel	*GetModel(unsigned int &uslot)
	{
		if (uslot < 0 || uslot >= models.size())
		{
			uslot = 0;
		}
		return models[uslot];
	}

	inline IDXGISwapChain	*pGetSwapChain()
	{
		return pSwapChain;
	}

	inline ID3D11RenderTargetView	*pGetRenderTarget()
	{
		return pRenderTarget;
	}
	inline ID3D11DepthStencilState	*pGetDepthStencilState()
	{
		return pDepthStencilState;
	}
	inline ID3D11DepthStencilState	*pGetDisabledDepthStencilState()
	{
		return pDepthStencilStateDisabled;
	}
	inline ID3D11DepthStencilView	*pGetDepthStencilView()
	{
		return pDepthStencilView;
	}
	inline ID3D11Texture2D			*pGetDepthStencil()
	{
		return pDepthStencil;
	}
	inline ID3D11Device				*pGetDevice()
	{
		return pDevice;
	}
	inline ID3D11DeviceContext		*pGetDeviceContext()
	{
		return pDeviceContext;
	}


private:

	// Private members of this class
	int *pRefreshRateHZ;

	int iVRAM;
	int iRAM;

	float fFov;
	float fAspect;
	float fRotationAngle;

	unsigned int uRealRefreshRates;
	unsigned int uModes;
	unsigned int uStringLength;
	unsigned int uNumerator;
	unsigned int uDenominator;

	char cVGADesc[128];

	IDXGISwapChain					*pSwapChain;
	IDXGIFactory					*pFactory;
	IDXGIAdapter					*pAdapter;
	IDXGIOutput						*pAdapterOutput;

	ID3D11Device					*pDevice;
	ID3D11DeviceContext				*pDeviceContext;
	ID3D11RenderTargetView			*pRenderTarget;
	ID3D11Texture2D					*pDepthStencil;
	ID3D11DepthStencilView			*pDepthStencilView;
	ID3D11DepthStencilState			*pDepthStencilState;
	ID3D11DepthStencilState			*pDepthStencilStateDisabled;
	ID3D11Buffer					*pPointLights;

	DXGI_MODE_DESC					*pModeDesc;
	DXGI_ADAPTER_DESC				 AdapterDesc;

	vector <CModel*> models;

	CCamera   *camera;
	CImporter *importer;
	CShader	  *shader;
	CSprite	  *sprite;
	// Private functions of this class

	bool	GetsSystemInfo(int width, int height);

	void    WriteToFile(int, int);

	bool	CreatesDeviceAndSwapChain(int width, int height, HWND hWnd);
	bool	CreatesBuffers(int width, int height);
	
	void	CreatesViewports(int width, int height);

	bool	SetsBuffers(unsigned int index);
	bool	ImportsTextures(unsigned int index);

	void	SetsScene(int width, int height);
	
};

#endif