#include "CModel.h"

CModel::CModel()
{
}

CModel::CModel(const CModel& any)
{
}

CModel::~CModel()
{
}

void CModel::InitMaterial(vector <ID3D11ShaderResourceView*> *pTextures, vector <string> *pTexNames, D3DXVECTOR3 mKd, D3DXVECTOR3 mKa, D3DXVECTOR3 mKs)
{
	material = new CMaterial;

	material->SetTextureVector(pTextures);

	material->SetTextureNamesVector(pTexNames);

	material->SetMaterialKa(mKa);

	material->SetMaterialKs(mKs);

	material->SetMaterialKd(mKd);

	return;
}

void CModel::InitMaterial()
{
	material = new CMaterial;

	return;
}

void CModel::DestroyModel()
{
	if (material->pGetTextures()->size() > 0)
		{
			unsigned int uTextSize = material->GetNumTextures();

			for (unsigned int a = 0; a < uTextSize; a++)
			{
				material->pGetTextureByIndex(a)->Release();
			}
		}
		material->pGetTextures()->resize(0);
		
		if (pVertexBuffer)		pVertexBuffer->Release();

		if (pIndexBuffer)		pIndexBuffer->Release();

		delete material;
		material = 0;

	return;
}