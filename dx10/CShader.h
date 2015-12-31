#ifndef	CSHADER_H_
#define CSHADER_H_

// Includes
#include "Globals.h"
#include "CModel.h"
#include "CSprites.h"
#include "CCamera.h"
#include "Light.h"

class CShader
{
public:
	CShader();
	CShader(const CShader& any);
	~CShader();

	bool Initialize_new(ID3D11Device *pDev, LPWSTR fxFilename);
	bool RenderObject(ID3D11DeviceContext	*pDevCon, ID3D11Device *pDev, CModel *model, CCamera *camera,
						unsigned int stride, unsigned int offset);
	bool RenderSprite(ID3D11DeviceContext *pDevCon, CSprite *sprite, CCamera *camera);

	void Shutdown();

	bool SetShaderVariablesPerObject(ID3D11DeviceContext *pDevCon, CModel *model, unsigned int RenderMode);
	bool SetShaderVariablesPerFrame(ID3D11DeviceContext *pDevCon, CCamera *camera, CSprite *sprite, unsigned int RenderMode);

	inline void SetBufferAmount(unsigned int buffAm)
	{
		uBufferAmount = buffAm;
		return;
	}

private:

	ID3D11Buffer						*pPointLightBuffer;
	ID3D11Buffer						*pMaterialBuffer;
	ID3D11Buffer						*pWorldBuffer;
	ID3D11Buffer						*pViewProjBuffer;
	ID3D11Buffer						*pCameraBuffer;
	ID3D11SamplerState					*pLinearSampler;

	ID3D11VertexShader					*pVertexShader;
	ID3D11PixelShader					*pPixelShader;
	ID3D11InputLayout					*pInputLayout;


	unsigned int	uBufferAmount;

	bool CreateConstantBuffers(ID3D11Device	*pDev);
	bool CreateSamplerStates(ID3D11Device *pDev);
	bool CreateLayout(ID3D11Device	*pDev, ID3D10Blob	*pVertexShaderBuffer);
	bool OutputErrorMessage(ID3D10Blob	*errorMessage, LPWSTR shaderFilename);

	struct PerObject
	{
		D3DXMATRIX World;
	};

	struct PerFrame
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
	};

	struct PointLight
	{
		D3DXVECTOR4 Color;
		D3DXVECTOR3 Position;
		float Padding;
	};

	static const unsigned int MaxLights		= 128;

	struct cbPointLight
	{
		PointLight Lights[MaxLights];
		unsigned int NumLights;
		D3DXVECTOR3 Padding;
	};

	struct Material
	{
		D3DXVECTOR4 MatKd;
		D3DXVECTOR4 MatKa;
		D3DXVECTOR4 MatKs;
	};

	struct Camera
	{
		D3DXVECTOR3 Position;
		float padding;
	};
};

#endif