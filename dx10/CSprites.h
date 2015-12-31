#ifndef CSPRITES_H_
#define CSPRITES_H_

// Includes 
#include "Globals.h"

class CSprite
{
public:

	CSprite();
	CSprite(const CSprite& any);
	~CSprite();

	bool InitSprite(HWND hMainWindow, LPWSTR texturePath, unsigned int width, unsigned int height,
										ID3D11Device *pDevice,
										float znear, float depth);
	void Render(int x, int y);
	void Shutdown();
	
	D3DXMATRIX	orthoMatrix;
	D3DXMATRIX  worldMatrix;

	inline void SetCamNear(float znear)
	{
		zCamNear = znear;
		return;
	}
	inline void SetCamDepth(float zdepth)
	{
		zCamDepth = zdepth;
		return;
	}

	inline float GetCamNear()
	{	
		return zCamNear;
	}
	inline float GetCamDepth()
	{
		return zCamDepth;
	}

	inline unsigned int GetScrWidth()
	{
		return uWidth;
	}
	inline unsigned int GetScrHeight()
	{
		return uHeight;
	}

	inline ID3D11ShaderResourceView	*pGetTexture()
	{
		return pTexture;
	}

private:

	bool SetBuffers();
	bool SetTexture(LPWSTR texturePath);
	bool UpdateBuffers(int x, int y);

	void SetOrthoMatrix(D3DXMATRIX &_Out, unsigned int width, unsigned int height);

	ID3D11Device		*pDev;
	ID3D11DeviceContext *pDevCon;

	ID3D11Buffer				*pSpriteVBuffer;
	ID3D11Buffer				*pSpriteIBuffer;
	ID3D11ShaderResourceView	*pTexture;

	float zCamNear;
	float zCamDepth;

	int iPrevPosX;
	int iPrevPosY;

	unsigned int uSpriteWidth;
	unsigned int uSpriteHeight;
	unsigned int uWidth;
	unsigned int uHeight;

	struct SpriteVertex
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Normal;
		D3DXVECTOR2 Texture;
	};
};

#endif
