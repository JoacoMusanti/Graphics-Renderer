#include "CShader.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CShader::CShader()
{
	pPointLightBuffer			= NULL;
	pMaterialBuffer				= NULL;
	pWorldBuffer				= NULL;
	pViewProjBuffer				= NULL;
	pCameraBuffer				= NULL;
	pLinearSampler				= NULL;

	pVertexShader				= NULL;
	pPixelShader				= NULL;
	pInputLayout				= NULL;

	uBufferAmount				= NULL;
}

CShader::CShader(const CShader&)
{
}

CShader::~CShader()
{
}

bool CShader::Initialize_new(ID3D11Device *pDev, LPWSTR fxFilename)
{
	// This function is called to initialize the shader class
	ID3D10Blob	*pErrorMessage	= 0;
	ID3D10Blob	*pVertexBuff	= 0;
	ID3D10Blob  *pPixelBuff		= 0;
	// Compile the vertex shader using the Vertex Shader 4.0 version
	if ( FAILED(D3DX11CompileFromFile(fxFilename, 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 0,
		&pVertexBuff, &pErrorMessage, 0)) )
	{
		if (pErrorMessage)
		{
			return OutputErrorMessage(pErrorMessage, fxFilename);
		}
		else
		{
			return ::Error(L"Missing shader file");
		}
	}
	// Compile the pixel shader using the Pixel Shader 4.0 version
	if ( FAILED(D3DX11CompileFromFile(fxFilename, 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 0,
		&pPixelBuff, &pErrorMessage, 0)) )
	{
		if (pErrorMessage)
		{
			return OutputErrorMessage(pErrorMessage, fxFilename);
		}
		else
		{
			return ::Error(L"Missing shader file");
		}
	}
	// Set the pointers to our shaders
	if ( FAILED(pDev->CreateVertexShader(pVertexBuff->GetBufferPointer(), pVertexBuff->GetBufferSize(), 0, &pVertexShader)) )	return 
		::Error(L"Could not create vertex shader");

	if ( FAILED(pDev->CreatePixelShader(pPixelBuff->GetBufferPointer(), pPixelBuff->GetBufferSize(), 0, &pPixelShader)) )	return
		::Error(L"Could not create pixel shader");


	if (! CreateLayout(pDev, pVertexBuff) )		return false;
	// Release memory
	if (pErrorMessage)	pErrorMessage->Release();
	if (pVertexBuff)	pVertexBuff->Release();
	if (pPixelBuff)		pPixelBuff->Release();
	// Creates buffers and sampler states
	if (! CreateConstantBuffers(pDev) )		return false;
	if (! CreateSamplerStates(pDev) )		return false;

	return true;
}

bool CShader::RenderObject(ID3D11DeviceContext	*pDevCon, ID3D11Device *pDev,
								CModel *model, CCamera *camera, unsigned int stride, unsigned int offset)
{
	UNREFERENCED_PARAMETER(camera);
	UNREFERENCED_PARAMETER(pDev);

	// Send information to the video card
	pDevCon->IASetVertexBuffers(0, 1, model->ppGetVertexBuffer(), &stride, &offset);
	pDevCon->IASetIndexBuffer(model->pGetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	pDevCon->IASetInputLayout(pInputLayout);

	if (! SetShaderVariablesPerObject(pDevCon, model, RENDER_MODE_MODEL) )		return false;

	pDevCon->DrawIndexed(model->GetNumIndices(), 0, 0);

	return true;
}

bool CShader::RenderSprite(ID3D11DeviceContext *pDevCon, CSprite *sprite, CCamera *camera)
{
	UNREFERENCED_PARAMETER(camera);

	if (! SetShaderVariablesPerObject(pDevCon, 0, RENDER_MODE_SPRITE) )	return false;

	sprite->Render( 0, 0 );

	pDevCon->VSSetShader(pVertexShader, 0, 0);
	pDevCon->PSSetShader(pPixelShader, 0, 0);

	pDevCon->DrawIndexed(6, 0, 0);
	
	return true;
}

void CShader::Shutdown()
{
	// Free the class' memory
	if (pPointLightBuffer)	pPointLightBuffer->Release();
	if (pMaterialBuffer)	pMaterialBuffer->Release();
	if (pWorldBuffer)		pWorldBuffer->Release();
	if (pViewProjBuffer)	pViewProjBuffer->Release();
	if (pCameraBuffer)		pCameraBuffer->Release();
	if (pLinearSampler)		pLinearSampler->Release();
	if (pVertexShader)		pVertexShader->Release();
	if (pPixelShader)		pPixelShader->Release();
	if (pInputLayout)		pInputLayout->Release();
}

bool CShader::SetShaderVariablesPerObject(ID3D11DeviceContext *pDevCon, CModel *model, unsigned int RenderMode)
{
	// This function is called once per object
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	D3DXMATRIX world;

	if (model)
	{
		D3DXMatrixTranspose(&world, &model->worldMatrix);
	}
	else
	{
		D3DXMatrixIdentity(&world);
	}

	// Lock the buffer and set the world matrix of the object
	if ( FAILED(pDevCon->Map(pWorldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )
		return ::Error(L"Could not lock the buffer");

	PerObject *pData = static_cast<PerObject*>(mappedSubResource.pData);

	pData->World	    	= world;

	pDevCon->Unmap(pWorldBuffer, 0);
	
	pDevCon->VSSetConstantBuffers(1, 1, &pWorldBuffer); 


	if ( FAILED(pDevCon->Map(pMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )
		return ::Error(L"Could not lock the buffer");

	Material *pData2 = static_cast<Material*>(mappedSubResource.pData);

	pData2->MatKa	= static_cast<D3DXVECTOR4>(model->pGetMaterial()->GetKa());
	pData2->MatKd	= static_cast<D3DXVECTOR4>(model->pGetMaterial()->GetKd());
	pData2->MatKs	= static_cast<D3DXVECTOR4>(model->pGetMaterial()->GetKs());

	pDevCon->Unmap(pMaterialBuffer, 0);

	pDevCon->VSSetConstantBuffers(3, 1, &pMaterialBuffer);
	pDevCon->PSSetConstantBuffers(3, 1, &pMaterialBuffer);

	pDevCon->VSSetShader(pVertexShader, 0, 0);
	pDevCon->PSSetShader(pPixelShader, 0, 0);
	// Set sampler and textures
	pDevCon->PSSetSamplers(0, 1, &pLinearSampler);

	if (RenderMode == RENDER_MODE_MODEL)
	{
		for (unsigned int i = 0; i < model->pGetMaterial()->pGetTextures()->size(); i++)
		{
			pDevCon->PSSetShaderResources(i, 1, model->pGetMaterial()->ppGetTextureByIndex(i));
		}
	}
	else
	{
		return true;		
	}

	return true;
}

bool CShader::SetShaderVariablesPerFrame(ID3D11DeviceContext *pDevCon, CCamera *camera, CSprite *sprite, unsigned int RenderMode)
{
	// This function is called once per frame
	D3D11_MAPPED_SUBRESOURCE	mappedSubResource;
	D3DXMATRIX view;
	D3DXMATRIX pro;

	// Transpose the matrices
	D3DXMatrixTranspose(&view, &camera->viewMatrix);
	if (RenderMode == RENDER_MODE_MODEL)
	{
		D3DXMatrixTranspose(&pro, &camera->projectionMatrix);
	}
	else
	{
		D3DXMatrixTranspose(&pro, &sprite->orthoMatrix);
	}

	// Lock the buffer and pass the view and projection/orthogonal matrices
	if ( FAILED(pDevCon->Map(pViewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )
		return ::Error(L"Could not lock the buffer");

	PerFrame *pData = static_cast<PerFrame*>(mappedSubResource.pData);

	pData->View				= view;
	pData->Projection		= pro;

	pDevCon->Unmap(pViewProjBuffer, 0);

	pDevCon->VSSetConstantBuffers(0, 1, &pViewProjBuffer); 

	if ( FAILED(pDevCon->Map(pPointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )
		return ::Error(L"Could not lock the buffer");

	cbPointLight *pData2 = static_cast<cbPointLight*>(mappedSubResource.pData);

	pData2->Lights->Color	 = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	pData2->Lights->Position = D3DXVECTOR3(0.0f, 20.0f, 1.0f);
	pData2->NumLights		 = 1;

	pDevCon->Unmap(pPointLightBuffer, 0);

	pDevCon->VSSetConstantBuffers(2, 1, &pPointLightBuffer);
	pDevCon->PSSetConstantBuffers(2, 1, &pPointLightBuffer);

	if ( FAILED(pDevCon->Map(pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)) )
		return ::Error(L"Could not lock the buffer");

	Camera *pData3 = static_cast<Camera*>(mappedSubResource.pData);

	pData3->Position	= camera->getCamPos();

	pDevCon->Unmap(pCameraBuffer, 0);

	pDevCon->VSSetConstantBuffers(4, 1, &pCameraBuffer);

	return true;
}

bool CShader::CreateConstantBuffers(ID3D11Device	*pDev)
{
	// Create necessary buffers to pass variables to the video card
	D3D11_BUFFER_DESC	worldDesc;
	D3D11_BUFFER_DESC	viewProjDesc;
	D3D11_BUFFER_DESC	LightsDesc;
	D3D11_BUFFER_DESC	MaterialDesc;
	D3D11_BUFFER_DESC	CameraDesc;
	// Buffer that is used per object draw call
	worldDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	worldDesc.ByteWidth				= sizeof(PerObject);
	worldDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	worldDesc.MiscFlags				= 0;
	worldDesc.Usage					= D3D11_USAGE_DYNAMIC;
	worldDesc.StructureByteStride	= 0;

	if ( FAILED(pDev->CreateBuffer(&worldDesc, 0, &pWorldBuffer)) )	return
																::Error(L"Unable to create constant buffer");
	// Buffer that is used only once per frame
	viewProjDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	viewProjDesc.ByteWidth				= sizeof(PerFrame);
	viewProjDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	viewProjDesc.MiscFlags				= 0;
	viewProjDesc.Usage					= D3D11_USAGE_DYNAMIC;
	viewProjDesc.StructureByteStride	= 0;

	if ( FAILED(pDev->CreateBuffer(&viewProjDesc, 0, &pViewProjBuffer)) )	return	
																::Error(L"Unable to create constant buffer");

	// Buffer that is used to pass in the lights to de GPU
	LightsDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	LightsDesc.ByteWidth				= sizeof(cbPointLight);
	LightsDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	LightsDesc.MiscFlags				= 0;
	LightsDesc.Usage					= D3D11_USAGE_DYNAMIC;
	LightsDesc.StructureByteStride		= 0;

	if ( FAILED(pDev->CreateBuffer(&LightsDesc, 0, &pPointLightBuffer)) )	return
																::Error(L"Unable to create constant buffer");


	// Buffer that is used to pass in the material propieties
	MaterialDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	MaterialDesc.ByteWidth				= sizeof(Material);
	MaterialDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	MaterialDesc.MiscFlags				= 0;
	MaterialDesc.Usage					= D3D11_USAGE_DYNAMIC;
	MaterialDesc.StructureByteStride	= 0;


	if ( FAILED(pDev->CreateBuffer(&MaterialDesc, 0, &pMaterialBuffer)) )	return
																::Error(L"Unable to create constant buffer");

	// This is used to hold camera position and stuff
	CameraDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
	CameraDesc.ByteWidth				= sizeof(Camera);
	CameraDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	CameraDesc.MiscFlags				= 0;
	CameraDesc.Usage					= D3D11_USAGE_DYNAMIC;
	CameraDesc.StructureByteStride		= 0;

	if ( FAILED(pDev->CreateBuffer(&CameraDesc, 0, &pCameraBuffer)) )	return
																::Error(L"Unable to create constant buffer");
	return true;
}

bool CShader::CreateLayout(ID3D11Device *pDev, ID3D10Blob	*pVertexShaderBuffer)
{
	// Create an array describing the information of the vertices, position and color
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// Calculate the number of elements in layout array
	unsigned int nElements = (sizeof(layout) / sizeof(layout[0])); 

	if ( FAILED(pDev->CreateInputLayout(layout, nElements, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &pInputLayout)) )		return ::Error(L"Failed to create input layout");

	return true;
}

bool CShader::CreateSamplerStates(ID3D11Device	*pDev)
{
	// Create the sampler state to use in our shader
	D3D11_SAMPLER_DESC	SamplerDesc;
	ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));

	SamplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;		// Set all the UVWs to be between 0 and 1
	SamplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.MaxAnisotropy	= 1;
	SamplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;
	SamplerDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;

	if ( FAILED(pDev->CreateSamplerState(&SamplerDesc, &pLinearSampler)) )	return ::Error(L"Unable to create sampler state");

	return true;
}

bool CShader::OutputErrorMessage(ID3D10Blob	*errorMessage, LPWSTR shaderFilename)
{
	// This function will be called if a shader failed to compile
	// This function will output compiler errors and warnings to a text file
	ofstream out;
	char	*pError			= 0;
	unsigned int buffSize	= 0;
	
	buffSize = errorMessage->GetBufferSize();
	// Get the errors
	pError   = static_cast<char*>(errorMessage->GetBufferPointer());

	out.open("shader_output.txt", ios::out);
	// Output the errors
	for (unsigned int i = 0; i < buffSize; i++)
	{
		out << pError[i];
	}

	out.close();

	MessageBox(0, L"A shader failed to compile. Check shader_output.txt for more information", shaderFilename, MB_OK | MB_ICONERROR);

	return false;
}