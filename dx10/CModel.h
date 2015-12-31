#ifndef CMODEL_H_
#define CMODEL_H_

// Includes
// --------------------------------------------
#include "CMaterial.h"
// --------------------------------------------

using namespace std;

class CModel 
{
public:
	// Constructors and destructors for the class
	CModel();
	CModel(const CModel& any);
	~CModel();

	void InitMaterial(vector <ID3D11ShaderResourceView*> *pTextures, vector <string> *pTexNames, D3DXVECTOR3 mKd, D3DXVECTOR3 mKa, D3DXVECTOR3 mKs);
	void InitMaterial();
	void DestroyModel(); // This function also destroys the material

	// Inline functions definitions
	inline void TranslateModelTo(D3DXVECTOR3  Translate)
	{
		PivotPoint	= Translate;
	
		worldMatrix._41 = Translate.x;
		worldMatrix._42 = Translate.y;
		worldMatrix._43 = Translate.z;

		return;
	}
	inline void TranslateModelTo(float fX, float fY, float fZ)
	{
		PivotPoint.x = fX;
		PivotPoint.y = fY;
		PivotPoint.z = fZ;

		worldMatrix._41	= fX;
		worldMatrix._42	= fY;
		worldMatrix._43	= fZ;

		return;
	}
	inline void TranslateModelIncremental(float fX, float fY, float fZ)
	{
		PivotPoint.x += fX;
		PivotPoint.y += fY;
		PivotPoint.z += fZ;

		D3DXMATRIX	Translate;

		D3DXMatrixTranslation(&Translate, fX, fY, fZ);

		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &Translate);

		return;
	}
	inline void RotateModelX(float fAngle)
	{
		D3DXMATRIX Rotate;

		D3DXMatrixIdentity(&Rotate);
		
		worldMatrix._41 = 0;
		worldMatrix._42 = 0;
		worldMatrix._43 = 0;
		D3DXMatrixRotationX(&Rotate, static_cast<float>(D3DXToRadian(fAngle)));
		worldMatrix = Rotate * worldMatrix;
		worldMatrix._41 = PivotPoint.x;
		worldMatrix._42 = PivotPoint.y;
		worldMatrix._43 = PivotPoint.z;
		 
		return;
	}
	inline void RotateModelY(float fAngle)
	{
		D3DXMATRIX Rotate;

		D3DXMatrixIdentity(&Rotate);

		worldMatrix._41 = 0;
		worldMatrix._42 = 0;
		worldMatrix._43 = 0;
		D3DXMatrixRotationY(&Rotate, static_cast<float>(D3DXToRadian(fAngle)));
		worldMatrix = Rotate * worldMatrix;
		worldMatrix._41 = PivotPoint.x;
		worldMatrix._42 = PivotPoint.y;
		worldMatrix._43 = PivotPoint.z;

		return;
	}
	inline void RotateModelZ(float fAngle)
	{
		D3DXMATRIX Rotate;

		D3DXMatrixIdentity(&Rotate);

		worldMatrix._41 = 0;
		worldMatrix._42 = 0;
		worldMatrix._43 = 0;
		D3DXMatrixRotationZ(&Rotate, static_cast<float>(D3DXToRadian(fAngle)));
		worldMatrix = Rotate * worldMatrix;
		worldMatrix._41 = PivotPoint.x;
		worldMatrix._42 = PivotPoint.y;
		worldMatrix._43 = PivotPoint.z;

		return;
	}
	inline void SetNumIndices(DWORD uIndexCount)
	{
		uIndices	= uIndexCount;
	}
	inline void SetNumVertices(DWORD uVertexCount)
	{
		uVertices	= uVertexCount;
	}
	inline void SetPivotPoint(D3DXVECTOR3 pivot)
	{
		PivotPoint = pivot;
	}
	inline void SetOrigPosition(D3DXVECTOR3 orig)
	{
		OriginalPos = orig;
	}

	inline DWORD GetNumIndices()
	{
		return uIndices;
	}
	inline DWORD GetNumVertices()
	{
		return uVertices;
	}

	inline ID3D11Buffer *pGetVertexBuffer()
	{
		return pVertexBuffer;
	}
	inline ID3D11Buffer *pGetIndexBuffer()
	{
		return pIndexBuffer;
	}
	inline ID3D11Buffer **ppGetVertexBuffer()
	{
		return &pVertexBuffer;
	}
	inline ID3D11Buffer **ppGetIndexBuffer()
	{
		return &pIndexBuffer;
	}

	inline D3DXVECTOR3	GetPivotPoint()
	{
		return PivotPoint;
	}

	inline CMaterial	*pGetMaterial()
	{
		return material;
	}

	// Public variables, this stuff is read every frame

	D3DXMATRIX			worldMatrix;
	
private:
	// vertex buffer and index buffer pointers
	ID3D11Buffer						*pVertexBuffer;
	ID3D11Buffer						*pIndexBuffer;
	// DWORDs that hold the number of vertices and indices of the mesh
	unsigned int uIndices;
	unsigned int uVertices;
	// Vector3 that holds the pivot point
	D3DXVECTOR3		PivotPoint;
	D3DXVECTOR3     OriginalPos;
	
	CMaterial	*material;
};


#endif