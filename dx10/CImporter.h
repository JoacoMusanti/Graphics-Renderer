#ifndef CIMPORTER_H_
#define CIMPORTER_H_

// Includes
// --------------------------------------------
#include "Globals.h"
// --------------------------------------------

// Namespaces
// --------------------------------------------
using namespace std;
// --------------------------------------------

// Globals
// --------------------------------------------
struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 TextCoord;
};

typedef struct
{
	D3DXVECTOR3	ver;
	D3DXVECTOR3 norm;
	D3DXVECTOR2 tex;
} VECTOR8;
// --------------------------------------------

class CImporter
{
public:
	// Constructors and destructors for this class
	CImporter();
	CImporter(const CImporter&);
	~CImporter();

	// Public members
	bool bImportsVertices();
	bool bImportsVerticesByScanf();

	unsigned int uiGetsModelCount(string file);
	unsigned int uiGetsModelCountScanf(string file);

	SimpleVertex  *pVBuffer;
	DWORD         *pIBuffer;

	// Inline functions definitions

	inline unsigned int uiGetsNumVertices()
	{
		return iNumVertices;
	}
	inline unsigned int uiGetsNumIndices()
	{
		return iNumIndices;
	}

	inline void CleanTextures()
	{
		textures.resize(0);

		return;
	}

	inline vector <string>	&GetModelTextures()
	{
		vector <string> &pointer = textures;
	
		return pointer;
	}	

	inline D3DXVECTOR3	GetPivot()
	{
		return PivotPoint;
	}
	inline D3DXVECTOR3  GetMatKd()
	{
		return Kd;
	}
	inline D3DXVECTOR3  GetMatKa()
	{
		return Ka;
	}
	inline D3DXVECTOR3  GetMatKs()
	{
		return Ks;
	}

private:

	bool bCorruptedFile;

	ifstream     *pInputFile;

	string		  file_holder;

	int iNumVertices;
	int iNumIndices;
	int iNumMeshes;

	int last_index;

	vector <string>	 textures;

	D3DXVECTOR3	PivotPoint;
	D3DXVECTOR3 Kd;
	D3DXVECTOR3 Ka;
	D3DXVECTOR3 Ks;

	D3DXVECTOR3	GetPivotPoint(unsigned int uIndex);
	D3DXVECTOR3 GetKd(int &uIndex);
	D3DXVECTOR3 GetKa(int &uIndex);
	D3DXVECTOR3 GetKs(int &uIndex);
};




#endif