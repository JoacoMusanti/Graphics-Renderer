#include "CMaterial.h"

CMaterial::CMaterial()
{
	Textures.resize(0);
	TextureNames.resize(0);

	Mat_Kd.x	= 0.0f;
	Mat_Kd.y	= 0.0f;
	Mat_Kd.z	= 0.0f;

	Mat_Ka.x	= 0.0f;
	Mat_Ka.y	= 0.0f;
	Mat_Ka.z	= 0.0f;
	
	Mat_Ks.x	= 0.0f;
	Mat_Ka.y	= 0.0f;
	Mat_Ka.z	= 0.0f;

	fBouncyness		= 0;
	fTransparency	= 0;
	fReflectivity	= 0;
	fIllumination	= 0;
}

CMaterial::CMaterial(const CMaterial& any)
{
}

CMaterial::~CMaterial()
{
}