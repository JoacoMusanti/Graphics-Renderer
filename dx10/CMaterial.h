#pragma once

#ifndef CMATERIAL_H_
#define CMATERIAL_H_

// This class will take care of our textures, every mesh will have a pointer to an object of this class.

// Includes
#include "Globals.h"

// Namespaces 
using namespace std;

class CMaterial
{
public:
	CMaterial();
	CMaterial(const CMaterial& any);
	~CMaterial();

	inline void SetTextureVector(vector <ID3D11ShaderResourceView*> *pTex)
	{
		Textures = *pTex;

		return;
	}
	inline void SetTextureNamesVector(vector <string> *pTexNames)
	{
		TextureNames = *pTexNames;

		return;
	}
	inline void SetMaterialKd(D3DXVECTOR3 Kd)
	{
		Mat_Kd = Kd;

		return;
	}
	inline void SetMaterialKa(D3DXVECTOR3 Ka)
	{
		Mat_Ka = Ka;

		return;
	}
	inline void SetMaterialKs(D3DXVECTOR3 Ks)
	{
		Mat_Ks = Ks;

		return;
	}

	inline vector <ID3D11ShaderResourceView*>	*pGetTextures()
	{
		return &Textures;
	}
	inline vector <string>	*pGetTextureNames()
	{
		return &TextureNames;
	}

	inline ID3D11ShaderResourceView	*pGetTextureByIndex(unsigned int uIndex)
	{
		return Textures[uIndex];
	}
	inline ID3D11ShaderResourceView **ppGetTextureByIndex(unsigned int uIndex)
	{
		return &Textures[uIndex];
	}

	inline string	GetTextureNameByIndex(unsigned int uIndex)
	{
		return TextureNames[uIndex];
	}

	inline unsigned int GetNumTextures()
	{
		return Textures.size();
	}

	inline D3DXVECTOR3 GetKa()
	{
		return Mat_Ka;
	}
	inline D3DXVECTOR3 GetKd()
	{
		return Mat_Kd;
	}
	inline D3DXVECTOR3 GetKs()
	{
		return Mat_Ks;
	}

private:

	vector <ID3D11ShaderResourceView*>   Textures;
	vector <string>						 TextureNames;

	D3DXVECTOR3		Mat_Kd;			// 3 vectors that hold the Kd, Ka and Ks of the mesh's material
	D3DXVECTOR3		Mat_Ka;
	D3DXVECTOR3     Mat_Ks;

	float fBouncyness;
	float fTransparency;
	float fReflectivity;
	float fIllumination;
};

#endif