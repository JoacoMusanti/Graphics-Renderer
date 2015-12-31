// Includes
#include "CImporter.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CImporter::CImporter()
{
	pInputFile		= 0;
	pVBuffer		= 0;
	pIBuffer		= 0;
	iNumVertices	= 0;
	iNumIndices		= 0;
	iNumMeshes		= 0;
	last_index      = 0;

	bCorruptedFile  = false;

	file_holder     = "";

	textures.resize(0);
}

CImporter::CImporter(const CImporter& any)
{
}

CImporter::~CImporter()
{
}

bool CImporter::bImportsVertices()
{
	int first_find  = 0;
	int index       = 0;
	int counter     = 0;
	int nVertex		= 0;
	int nOptimized  = 0;
	int SpaceCountr = 0;
	int cVectorIndx = 0;
	int uFindSlot   = 0;

	iNumIndices			 = 0;
	iNumVertices		 = 0;
	iNumMeshes			 = 0;

	D3DXVECTOR3		vertex;
	D3DXVECTOR3     normal;

	D3DXVECTOR2     texture;

	string text_numb		 = "";


	if (file_holder == "")
	{
		MessageBox(NULL, L"File is corrupt, could not load models", L"ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	//
	// NOTHING TO SEE HERE, MOVE ALONG PLEASE
	//
	// SERIOUSLY, DO NOT TOUCH
	//

	index		= first_find  = file_holder.find("OBJECT", last_index);
	last_index  = file_holder.find("OBJECT", first_find + 1) - 1;

	PivotPoint = GetPivotPoint(index);

	if (last_index == string::npos - 1)
	{
		last_index = file_holder.length() - 1;
	}
	
	for (index; file_holder[index] != '|'; index++);

	index++;

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	textures.push_back(text_numb);
	text_numb = "";

	index++; // now get the number of vertices of the object

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	iNumVertices = iNumIndices = nVertex    = static_cast<unsigned int>(atoi(text_numb.c_str()));

	index++;
	text_numb = "";

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	nOptimized = static_cast<unsigned int>(atoi(text_numb.c_str()));

	vector <D3DXVECTOR3> vertex_pos(nOptimized);

	index++;
	text_numb = "";

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	nOptimized = static_cast<unsigned int>(atoi(text_numb.c_str()));

	vector <D3DXVECTOR2> texture_pos(nOptimized);

	index++;
	text_numb = "";

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	nOptimized = static_cast<unsigned int>(atoi(text_numb.c_str()));
	
	vector <D3DXVECTOR3> normal_pos(nOptimized);

	text_numb = "";
	index++;

	Ka = GetKa(index);
	Kd = GetKd(index);
	Ks = GetKs(index);

	if (bCorruptedFile)
	{
		MessageBox(NULL, L"File is corrupt, could not load models", L"ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	vector <unsigned int> vertex_indices(nVertex);
	vector <unsigned int> texture_indices(nVertex);
	vector <unsigned int> normal_indices(nVertex);

	pVBuffer = new SimpleVertex[nVertex];
	pIBuffer = new DWORD[nVertex];

	counter  = 0;

	while (index < last_index)
	{
		text_numb = "";
		while (file_holder[index] != '\n')
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				SpaceCountr++;
			}
			if (SpaceCountr == 3)
			{
				sscanf_s(text_numb.c_str(), "%f %f %f", &vertex.x, &vertex.y, &vertex.z, text_numb.size());
				SpaceCountr = 0;

				vertex_pos[cVectorIndx] = vertex;
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}

		index++;
		text_numb   = "";
		cVectorIndx = 0;

		while (file_holder[index] != '\n')
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				SpaceCountr++;
			}
			if (SpaceCountr == 2)
			{
				sscanf_s(text_numb.c_str(), "%f %f", &texture.x, &texture.y, text_numb.size());
				SpaceCountr = 0;

				texture_pos[cVectorIndx] = texture;
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}	

		index++;
		text_numb   = "";
		cVectorIndx = 0;

		while (file_holder[index] != '\n')
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				SpaceCountr++;
			}
			if (SpaceCountr == 3)
			{
				sscanf_s(text_numb.c_str(), "%f %f %f", &normal.x, &normal.y, &normal.z, text_numb.size());
				SpaceCountr = 0;

				normal_pos[cVectorIndx] = normal;
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}	

		index++;
		text_numb   = "";
		cVectorIndx = 0;

		while (cVectorIndx != nVertex)
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				vertex_indices[cVectorIndx] = static_cast<unsigned int>(atoi(text_numb.c_str()));
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}

		text_numb   = "";
		cVectorIndx = 0;

		while (cVectorIndx != nVertex)
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				texture_indices[cVectorIndx] = static_cast<unsigned int>(atoi(text_numb.c_str()));
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}

		text_numb   = "";
		cVectorIndx = 0;

		while (cVectorIndx != nVertex)
		{
			text_numb += file_holder[index];
			index++;

			if (file_holder[index] == ' ')
			{
				normal_indices[cVectorIndx] = static_cast<unsigned int>(atoi(text_numb.c_str()));
				cVectorIndx++;

				text_numb = "";
				index++;
			}
		}
	}

	for (unsigned int v = 0; v < vertex_indices.size(); v++)
	{
		pVBuffer[v].Pos			= vertex_pos[vertex_indices[v]];
		pVBuffer[v].TextCoord	= texture_pos[texture_indices[v]];
		pVBuffer[v].Normal		= normal_pos[normal_indices[v]];

		pIBuffer[v] = v;
	}

	return true;
}

bool CImporter::bImportsVerticesByScanf()
{
	int first_find  = 0;
	int index       = 0;
	int counter     = 0;

	char	*buffer			 = 0;

	string text_numb		 = "";
	string line_holder       = "";

	vector <VECTOR8> vertex_texcoords;
	vertex_texcoords.resize(0);

	VECTOR8 all_coords;

	if (file_holder == "")
	{
		MessageBox(NULL, L"File is corrupt, could not load models", L"ERROR", MB_OK | MB_ICONERROR);
		return false;
	}

	index		= first_find  = file_holder.find("OBJECT", last_index);
	last_index  = file_holder.find("OBJECT", first_find + 1) - 1;

	if (last_index == string::npos - 1)
	{
		last_index = file_holder.length();
	}
	
	for (index; file_holder[index] != '|'; index++);

	index++;

	while (file_holder[index] != '\n')
	{
		text_numb += file_holder[index];
		index++;
	}

	textures.push_back(text_numb);
	text_numb = "";

	index++; // Start parsing vertex and texture coordinates

	while (index < last_index)
	{
		int line_size = 0;

		for (line_size; file_holder[index] != '\n'; line_size++)
		{
			line_holder += file_holder[index];
			index++;
		}

		index++;

		sscanf_s(line_holder.c_str(), "%f %f %f %f %f %f %f %f", &all_coords.ver.x, &all_coords.ver.y, &all_coords.ver.z, &all_coords.tex.x, &all_coords.tex.y,
			&all_coords.norm.x, &all_coords.norm.y, &all_coords.norm.z);

		vertex_texcoords.push_back(all_coords);

		delete[] buffer;
		buffer = 0;

		line_holder = "";
	}

	pVBuffer = new SimpleVertex[vertex_texcoords.size()];
	pIBuffer = new DWORD[vertex_texcoords.size()];

	for (unsigned int i = 0; i < vertex_texcoords.size(); i++)
	{
		pIBuffer[i] = i;

		pVBuffer[i].Pos.x  =  vertex_texcoords[i].ver.x;
		pVBuffer[i].Pos.y  =  vertex_texcoords[i].ver.y;
		pVBuffer[i].Pos.z  =  vertex_texcoords[i].ver.z;

		pVBuffer[i].TextCoord.x  =  vertex_texcoords[i].tex.x;
		pVBuffer[i].TextCoord.y  =  vertex_texcoords[i].tex.y;
	}

	iNumIndices  =	vertex_texcoords.size();
	iNumVertices = vertex_texcoords.size();

	return true;

}

unsigned int CImporter::uiGetsModelCount(string file)
{
	string line_holder  = "";
	unsigned int pos    = 0;

	pInputFile = new ifstream;

	pInputFile->open(file, ios::in | ios::beg);

	if ( !pInputFile->is_open() )
	{
		MessageBox(NULL, L"Could not open the *.qdv file, make sure you are running the program from the root folder",
			L"ERROR", MB_OK | MB_ICONERROR);
		return 0;
	}

	unsigned int vertex_count = 0;

	while (!pInputFile->eof())
	{
		getline(*pInputFile, line_holder);
		file_holder += line_holder + '\n';
		vertex_count++;
	}

	pInputFile->close();

	delete pInputFile;
	pInputFile = 0;



	line_holder = ""; 
	pos = file_holder.find("MODELS");

	if (pos != string::npos)
	{
		pos += 7;
		while (file_holder[pos] != '\n')
		{
			line_holder += file_holder[pos];
			pos++;
		}
	}

	// Delete our file object as we do not need it anymore

	return atoi(line_holder.c_str());
}

unsigned int CImporter::uiGetsModelCountScanf(string file)
{

	// SHIT'S BROKEN, TO DO: Mix both sscanf_s() and our algorithm to work better and faster in a combined mode

	string line = "";

	ifstream input_file;

	input_file.open(file);

	if (!input_file.is_open())		return 0;

	while (!input_file.eof())
	{
		getline(input_file, line, '\n');

		//lines_of_file.push_back(line);
	}

	input_file.close();

	//char	*buffer = new char[lines_of_file[0].size()];

	//delete[] buffer;

	return iNumMeshes;		//=  sscanf_s(buffer, "%d");
}

D3DXVECTOR3		CImporter::GetPivotPoint(unsigned int uIndex)
{
	int uCounter = uIndex;
	
	D3DXVECTOR3		Pivot(0.0f, 0.0f, 0.0f);

	string	sPivot;

	for (uCounter; file_holder[uCounter] != ' '; uCounter++);

	for (uCounter; file_holder[uCounter] != '\n'; uCounter++)
	{
		sPivot += file_holder[uCounter];
	}

	sscanf_s(sPivot.c_str(), "%f %f %f", &Pivot.x, &Pivot.y, &Pivot.z);

	return Pivot;
}

D3DXVECTOR3		CImporter::GetKd(int &uIndex)
{
	D3DXVECTOR3	Return(0.0f, 0.0f, 0.0f);

	string sKd	= "";

	if (file_holder[uIndex] != 'K')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;
	if (file_holder[uIndex] != 'd')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;

	for (uIndex; file_holder[uIndex] != '\n'; uIndex++)
	{
		sKd += file_holder[uIndex];
	}

	uIndex++;

	sscanf_s(sKd.c_str(), "%f %f %f", &Return.x, &Return.y, &Return.z);

	return Return;
}

D3DXVECTOR3		CImporter::GetKa(int &uIndex)
{
	D3DXVECTOR3	Return(0.0f, 0.0f, 0.0f);

	string sKa	= "";

	if (file_holder[uIndex] != 'K')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;
	if (file_holder[uIndex] != 'a')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;

	for (uIndex; file_holder[uIndex] != '\n'; uIndex++)
	{
		sKa += file_holder[uIndex];
	}

	uIndex++;

	sscanf_s(sKa.c_str(), "%f %f %f", &Return.x, &Return.y, &Return.z);

	return Return;
}

D3DXVECTOR3		CImporter::GetKs(int &uIndex)
{
	D3DXVECTOR3	Return(0.0f, 0.0f, 0.0f);

	string sKs	= "";

	if (file_holder[uIndex] != 'K')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;
	if (file_holder[uIndex] != 's')
	{
		bCorruptedFile = true;
		D3DXVECTOR3 Failed(0.0f, 0.0f, 0.0f);
		return Failed;
	}
	uIndex++;

	for (uIndex; file_holder[uIndex] != '\n'; uIndex++)
	{
		sKs += file_holder[uIndex];
	}

	uIndex++;

	sscanf_s(sKs.c_str(), "%f %f %f", &Return.x, &Return.y, &Return.z);

	return Return;
}