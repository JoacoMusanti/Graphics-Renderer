#include "CSprites.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CSprite::CSprite()
{
	pDev			= 0;
	pSpriteIBuffer	= 0;
	pSpriteVBuffer	= 0;
}

CSprite::CSprite(const CSprite& any)
{
}

CSprite::~CSprite()
{
}

bool CSprite::InitSprite(HWND hMainWindow, LPWSTR texturePath, unsigned int width, unsigned int height,
											 ID3D11Device *pdevice,
											 float znear, float depth)
{
	RECT rect;

	GetClientRect(hMainWindow, &rect);

	uWidth	= rect.right;
	uHeight = rect.bottom;

	SetCamNear(znear);
	SetCamDepth(depth);

	iPrevPosX	= -1;
	iPrevPosY	= -1;

	uSpriteWidth  = width;
	uSpriteHeight = height;

	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixIdentity(&orthoMatrix);

	pDev = pdevice;

	if (! SetBuffers() )	return false;
	
	if (! SetTexture(texturePath) )	return false;

	SetOrthoMatrix(orthoMatrix, width, height);

	return true;
}

void CSprite::Render(int x, int y)
{
	unsigned int stride = sizeof(SpriteVertex);
	unsigned int offset = 0;

	//pDevCon->IASetInputLayout(pInputLayout);
	pDevCon->IASetVertexBuffers(0, 1, &pSpriteVBuffer, &stride, &offset);
	pDevCon->IASetIndexBuffer(pSpriteIBuffer, DXGI_FORMAT_R32_UINT, offset);

	UpdateBuffers(x, y);

	return;
}

bool CSprite::UpdateBuffers(int x, int y)
{
	if (x == iPrevPosX)
	{
		if (y == iPrevPosY)
		{
			return true;
		}
	}

	iPrevPosX = x;
	iPrevPosY = y;

	float fLeft		= static_cast<float>(uWidth) / -2 + x;
	float fRight	= fLeft + static_cast<float>(uSpriteWidth);
	float fTop		= static_cast<float>(uHeight) / 2 - y ;
	float fBottom	= fTop - static_cast<float>(uSpriteHeight);

	D3D11_MAPPED_SUBRESOURCE	mappedSubResource;
	SpriteVertex	*pVertices	= new SpriteVertex[4];

	if ( FAILED(pDevCon->Map(pSpriteVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )		return false;

	pVertices	= static_cast<SpriteVertex*>(mappedSubResource.pData);

	pVertices[0].Position = D3DXVECTOR3(fLeft, fTop, 0.0f);
	pVertices[0].Texture  =	D3DXVECTOR2(0.0f, 0.0f);
	pVertices[0].Normal	  = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVertices[1].Position = D3DXVECTOR3(fRight, fTop, 0.0f);
	pVertices[1].Texture  =	D3DXVECTOR2(1.0f, 0.0f);
	pVertices[1].Normal	  = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVertices[2].Position = D3DXVECTOR3(fRight, fBottom, 0.0f);
	pVertices[2].Texture  =	D3DXVECTOR2(1.0f, 1.0f);
	pVertices[2].Normal	  = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVertices[3].Position = D3DXVECTOR3(fLeft, fBottom, 0.0f);
	pVertices[3].Texture  =	D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].Normal	  = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pDevCon->Unmap(pSpriteVBuffer, 0);

	delete[] pVertices;
	pVertices = 0;

	return true;
}

bool CSprite::SetBuffers()
{
	// We will create a quad, formed by two trangles, centered around the origin

	unsigned long	Indices[] =
	{
		// Triangle
		0, 1, 3, 
		// Triangle
		1, 2, 3,
	};

	// Create the descriptions for the two buffers
	D3D11_BUFFER_DESC	vertexDesc;
	D3D11_BUFFER_DESC   indexDesc;

	D3D11_SUBRESOURCE_DATA  indexSubData;

	// Fill out the structures
	vertexDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth			= sizeof(SpriteVertex) * 4;
	vertexDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	vertexDesc.MiscFlags			= 0;
	vertexDesc.Usage				= D3D11_USAGE_DYNAMIC;
	vertexDesc.StructureByteStride	= 0;

	if ( FAILED(pDev->CreateBuffer(&vertexDesc, 0, &pSpriteVBuffer)) )		return false;

	indexDesc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth				= sizeof(unsigned long) * 6;
	indexDesc.CPUAccessFlags		= 0;
	indexDesc.MiscFlags				= 0;
	indexDesc.Usage					= D3D11_USAGE_DEFAULT;
	indexDesc.StructureByteStride	= 0;

	indexSubData.pSysMem	= Indices;

	if ( FAILED(pDev->CreateBuffer(&indexDesc, &indexSubData, &pSpriteIBuffer)) )		return false;

	return true;
}

bool CSprite::SetTexture(LPWSTR textPath)
{
	if ( FAILED(D3DX11CreateShaderResourceViewFromFile(pDev, textPath, 0, 0, &pTexture, 0)) )	return false; 

	return true;
}

void CSprite::Shutdown()
{
	if (pSpriteIBuffer)		pSpriteIBuffer->Release();
	if (pSpriteVBuffer)		pSpriteVBuffer->Release();
	if (pTexture)			pTexture->Release();

	return;
}

void CSprite::SetOrthoMatrix(D3DXMATRIX &_out, unsigned int width, unsigned int height)
{
	D3DXMatrixOrthoLH(&_out, static_cast<float>(width), static_cast<float>(height), zCamNear, zCamDepth);
	return;
}