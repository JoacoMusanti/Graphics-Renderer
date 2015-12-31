// Includes 
// ------------------------------------------
#include "CGraphics.h"
// ------------------------------------------

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

CGraphics::CGraphics()
{
	// In the constructor we nullify all the pointers in our CGraphics class
	pSwapChain					= NULL;
	pDevice						= NULL;
	pDeviceContext				= NULL;
	pRenderTarget				= NULL;
	pDepthStencil				= NULL;
	pDepthStencilView			= NULL;
	pDepthStencilState			= NULL;
	pDepthStencilStateDisabled	= NULL;
	pFactory					= NULL;
	pAdapter					= NULL;
	pAdapterOutput				= NULL;
	pModeDesc					= NULL;	
	pPointLights				= NULL;

	shader					= NULL;
	camera					= NULL;
	importer				= NULL;
	sprite					= NULL;

	models.resize(0);
}

CGraphics::CGraphics(const CGraphics&)
{
	// not used
}

CGraphics::~CGraphics()
{
	// not used
}

bool CGraphics::InitGraph(int& width, int& height, HWND hWnd)
{
	unsigned int nMeshes = 0;

	if ( FAILED( GetsSystemInfo(width, height)) )		return ::Error(L"The program failed to retrieve system info");

	WriteToFile(width, height);
	
	if ( !CreatesDeviceAndSwapChain(width, height, hWnd) )			return ::Error(L"The program failed to create the Direct3D device");

	if ( !CreatesBuffers(width, height) )	  return false;

	CreatesViewports(width, height);

	D3DXVECTOR4	useForEverything(0.5f, 0.5f, 0.5f, 0.5f);
	D3DXVECTOR3 useForEv(0.5f, 0.5f, 0.5f);

	camera		 = new CCamera;
	importer	 = new CImporter;
	shader		 = new CShader;
	sprite		 = new CSprite;

	LPWSTR Path = L"C://Users/Joaquin/Documents/Visual Studio 2010/Projects/dx10/Debug/texture.jpg";

	sprite->InitSprite(hWnd, Path, width, height, pDevice, ::gCamNear, ::gCamDepth);

	nMeshes = importer->uiGetsModelCount("shapes.qdv");
	if (nMeshes == 0)		return ::Error(L"No meshes were found in the file, the file is probably corrupt");

	if ( !shader->Initialize_new(pDevice, L"effect.fx") )
														return false;

	//HavokManager->Initialize();

	models.resize(nMeshes);

	for (unsigned int i = 0; i < nMeshes; i++)
	{
		models[i]	= new CModel;

		models[i]->InitMaterial(); 
	
		if ( !importer->bImportsVertices() )		return ::Error(L"Unable to open qdv file, make sure you are running the program "
																	 L"from the root folder");
		if ( !SetsBuffers(i) )	return false;

		if ( !ImportsTextures(i) )	return ::Error(L"One texture failed to load");
		
		if (importer->pIBuffer)
		{
			delete[] importer->pIBuffer;
			importer->pIBuffer  = 0;
		}
		if (importer->pVBuffer)
		{
			delete[] importer->pVBuffer;	
			importer->pVBuffer = 0;
		}

		D3DXMatrixIdentity(&models[i]->worldMatrix);
		models[i]->TranslateModelTo(importer->GetPivot());
		models[i]->SetOrigPosition(importer->GetPivot());
		models[i]->pGetMaterial()->SetMaterialKa(importer->GetMatKa());
		models[i]->pGetMaterial()->SetMaterialKd(importer->GetMatKd());
		models[i]->pGetMaterial()->SetMaterialKs(importer->GetMatKs());
	}

	SetsScene( width, height );
	
	// Do some clean up

	delete	 importer;
	importer = 0;

	return true;
}

void CGraphics::ShutdownGraph()
{
	//HavokManager->Shutdown();

	if (pPointLights)				pPointLights->Release();

	if (pAdapterOutput)				pAdapterOutput->Release();

	if (pAdapter)					pAdapter->Release();

	if (pFactory)					pFactory->Release();

	if (pRenderTarget)				pRenderTarget->Release();

	if (pDepthStencilState)			pDepthStencilState->Release();

	if (pDepthStencilStateDisabled)	pDepthStencilStateDisabled->Release();

	if (pDepthStencil)				pDepthStencil->Release();	

	if (pDepthStencilView)			pDepthStencilView->Release();

	if (pSwapChain)					
	{
		if (::gFullScreen)
		{
			pSwapChain->SetFullscreenState(false, 0);
			ChangeDisplaySettings(0, 0);
		}
		pSwapChain->Release();
	}

	if (shader)		shader->Shutdown();

	if (sprite)		sprite->Shutdown();

	for (unsigned int i = 0; i < models.size(); i++)
	{
		models[i]->DestroyModel();

		delete models[i];
		models[i] = 0;
	}

	if (pDeviceContext)				pDeviceContext->Release();

	if (pDevice)					pDevice->Release();

	models.resize(0);

	delete pRefreshRateHZ;
	pRefreshRateHZ = 0;

	delete camera;
	camera	 = 0;

	delete shader;
	shader	 = 0;

	delete sprite;
	sprite	 = 0;

	#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
	#endif

	return;
}

bool CGraphics::Render()
{
	unsigned int stride = sizeof(SimpleVertex);
	unsigned int offset = 0;

	// Clear the render target
	pDeviceContext->ClearRenderTargetView(pRenderTarget, D3DXCOLOR(0.0f, 0.0f, 0.2f, 0.0f));
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0, 0);


	if (::gLoadingScreen)
	{
		if (! shader->SetShaderVariablesPerFrame(pDeviceContext, camera, sprite, RENDER_MODE_SPRITE) )
					return ::Error(L"Failed to send variables to video card");
		shader->RenderSprite(pDeviceContext, sprite, camera);
	}
	else
	{
		if (! shader->SetShaderVariablesPerFrame(pDeviceContext, camera, 0, RENDER_MODE_MODEL) )
					return ::Error(L"Failed to send variables to video card");

		for (unsigned int count = 0; count < models.size(); count++)
		{
			shader->RenderObject(pDeviceContext, pDevice, models[count], camera, stride, offset);
		}
	}

	// Swap the buffers
	pSwapChain->Present(0, 0);

	return true;
}

bool CGraphics::GetsSystemInfo(int width, int height)
{
	// This function is used to retrieve the refresh rate of the monitor, video card description and memory

	// First we have to create the DirectX interface Factory
	// If failed, return false
	if (FAILED( CreateDXGIFactory(__uuidof(IDXGIFactory), (LPVOID*) &pFactory)))
	{
		return false;
	}
	// Then we have to enumerate the video cards
	if (FAILED( pFactory->EnumAdapters(0, &pAdapter) ))
	{
		return false;
	}
	// We can now enumerate the outputs (monitors)
	if (FAILED( pAdapter->EnumOutputs(0, &pAdapterOutput) ))
	{
		return false;
	}
	// Then we have to get the display mode list and store the number of modes in mModes
	if (FAILED( pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &uModes, NULL) ))
	{
		return false;
	}
	// Create a list to hold all the possible display modes for the vga/monitor
	pModeDesc = new DXGI_MODE_DESC[uModes];
	if (!pModeDesc)
	{
		return false;
	}
	// Fill the display structures
	if (FAILED( pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &uModes, pModeDesc) ))
	{
		return false;
	}
	// Initialize an array that will hold the refresh rate in Hz for later output to a text file
	pRefreshRateHZ		= new int[uModes];
	uRealRefreshRates	= 0;
	// Create a new loop which loops through every X axis mode and Y axis mode and if they are compatible, store the refresh rate
	// of that mode in mNumerator and mDenomnator. We also save the quotient of the numerator and denominator
	for (unsigned int i = 0; i < uModes; i++)
	{
		if (pModeDesc[i].Width == static_cast<unsigned int>(width))
		{
			if (pModeDesc[i].Height == static_cast<unsigned int>(height))
			{
				uNumerator					      = pModeDesc[i].RefreshRate.Numerator;
				uDenominator					  = pModeDesc[i].RefreshRate.Denominator;
				pRefreshRateHZ[uRealRefreshRates] = uNumerator / uDenominator;				// Calculate the real number of the refresh rate in Hz
				uRealRefreshRates++;
			}
		}
	}

	// Get description of our video card and store it in AdapterDesc
	if (FAILED( pAdapter->GetDesc(&AdapterDesc) ))
	{
		return false;
	}
	// Store the amount of VRAM of the videocard in iVRAM (in megabytes)
	iVRAM = static_cast<int> ((AdapterDesc.DedicatedVideoMemory / 1024 / 1024));
	// Convert the name of the vga to a character array and store it
	if (wcstombs_s(&uStringLength, cVGADesc, 128, AdapterDesc.Description, 128) != 0)
	{
		return false;
	}

	// Start releasing all the pointers used in this function, as they are no longer used
	pFactory->Release();
	pFactory = 0;

	pAdapter->Release();
	pAdapter = 0;

	pAdapterOutput->Release();
	pAdapterOutput = 0;

	delete pModeDesc;
	pModeDesc = 0;

	// return true
	return true;
}

void CGraphics::WriteToFile(int width, int height)
{
	// This function will dump all the info we retrieved in GetSystemInfo, and store it into a text file
	// First we create an ofstream object
	ofstream info;
	// Then we open it and name it, the trunc flag will delete previous files named the same
	info.open("display.txt", ios::trunc);
	// Output a string 
	info << "MONITOR FREQUENCY = ";
	// And after that output ALL the possible refresh rates
	for (unsigned int i = 0; i < uRealRefreshRates; i++)
	{
		info << pRefreshRateHZ[i];					// Output a refresh rate
		if ((i+1) != uRealRefreshRates) 			// If the next one is not he last one
		{
			info << "/";							// Separate it from the followind with a slash
		}
	}
	info << "(used)";								// Indicate that the last one was used
	// In hertz
	info << " Hz";
	// Show the resolution used
	info << "\nRESOLUTION USED = ";
	info << width;
	info << "x";
	info << height;
	// Input another string
	info << "\nVIDEO CARD = ";
	// And output the vga name
	info << cVGADesc;
	// Output another string
	info << "\nVGA MEMORY = ";
	// And then output the vga memory in megabytes
	info << iVRAM;
	info << " MB";
	// Close the file
	info.close();
	// return
	return;
}

bool CGraphics::CreatesDeviceAndSwapChain(int width, int height, HWND hWnd)
{
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// Clear all memory of the struct so we can safely leave some struct members at 0
	ZeroMemory( &swapChainDesc, sizeof(swapChainDesc) );

	
	swapChainDesc.BufferCount							= 1;								// We will only use 1 back buffer
	swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;		// Using 8 bits for each channel per pixel
	swapChainDesc.BufferDesc.Height						= height;							// height of the window 
	swapChainDesc.BufferDesc.Width						= width;							// width of the window
	if (::gVSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= uNumerator;						
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= uDenominator;	
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;						
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	}
	swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;	// We will be using the buffer for the render target output
	swapChainDesc.OutputWindow							= hWnd;								// handle to our window
	swapChainDesc.SampleDesc.Count						= 1;								// 1 sample
	swapChainDesc.SampleDesc.Quality					= 0;								// 0 quality
	swapChainDesc.Flags									= 0;
	if (::gFullScreen)
	{
		swapChainDesc.Windowed							= false;
	}
	else
	{
		swapChainDesc.Windowed							= true;
	}
	swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;


	// Set the rendering modes
	D3D_DRIVER_TYPE	driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL featureLevel[] =
	{
		D3D_FEATURE_LEVEL_10_0,
	};
	
	int numTypes = (sizeof(driverTypes) / sizeof(driverTypes[0]));
	
	// Try to render with the video card, if fails, render with cpu.
	for (unsigned int i = 0; i < static_cast<unsigned int>(numTypes); i++)
	{
		result = D3D11CreateDeviceAndSwapChain(0, driverTypes[i], 0, 0, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
			&pSwapChain, &pDevice, 0, &pDeviceContext);
		if (SUCCEEDED(result))
		{
			break;
		}
	}
	// If it failed 2 times, we return false
	if ( FAILED(result) )		return	false;

	return true;
}

bool CGraphics::CreatesBuffers(int width, int height)
{
	ID3D11Texture2D	*pBackBuffer;
	// Call GetBuffer, if fails, return false
	if ( FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer)) )		return Error(L"Unable to get buffer from swap chain");

	if ( FAILED(pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTarget)) )		return Error(L"Unable to create render target");

	pBackBuffer->Release();

	D3D11_TEXTURE2D_DESC	DepthDesc;

	ZeroMemory(&DepthDesc, sizeof(DepthDesc));

	DepthDesc.Width				 = width;
	DepthDesc.Height			 = height;
	DepthDesc.MipLevels			 = 1;
	DepthDesc.ArraySize			 = 1;
	DepthDesc.Format			 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthDesc.SampleDesc.Count	 = 1;
	DepthDesc.SampleDesc.Quality = 0;
	DepthDesc.Usage				 = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags			 = D3D11_BIND_DEPTH_STENCIL;
	DepthDesc.CPUAccessFlags	 = 0;
	DepthDesc.MiscFlags			 = 0;

	if ( FAILED(pDevice->CreateTexture2D(&DepthDesc, NULL, &pDepthStencil)) )	return Error(L"Unable to create "
																							  L"the depth buffer");

	pDeviceContext->OMSetDepthStencilState(pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC	DepthViewDesc;

	ZeroMemory(&DepthViewDesc, sizeof(DepthViewDesc));

	DepthViewDesc.Format			 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthViewDesc.ViewDimension		 = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthViewDesc.Texture2D.MipSlice = 0;

	if ( FAILED(pDevice->CreateDepthStencilView(pDepthStencil, &DepthViewDesc, &pDepthStencilView)) )	 return Error(L"Unable to create "
																													   L"the depth stencil view");
	
	D3D11_DEPTH_STENCIL_DESC	DepthStencilStateDesc;

	ZeroMemory(&DepthStencilStateDesc, sizeof(DepthStencilStateDesc));

	DepthStencilStateDesc.DepthEnable		= true;
	DepthStencilStateDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilStateDesc.DepthFunc			= D3D11_COMPARISON_LESS;

	DepthStencilStateDesc.StencilEnable		= true;
	DepthStencilStateDesc.StencilReadMask	= 0xFF;
	DepthStencilStateDesc.StencilWriteMask	= 0xFF;

	DepthStencilStateDesc.FrontFace.StencilFailOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.FrontFace.StencilDepthFailOp	=   D3D11_STENCIL_OP_INCR;
	DepthStencilStateDesc.FrontFace.StencilPassOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.FrontFace.StencilFunc			=	D3D11_COMPARISON_ALWAYS;

	DepthStencilStateDesc.BackFace.StencilFailOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.BackFace.StencilDepthFailOp	=	D3D11_STENCIL_OP_DECR;
	DepthStencilStateDesc.BackFace.StencilPassOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.BackFace.StencilFunc			=	D3D11_COMPARISON_ALWAYS;

	if ( FAILED(pDevice->CreateDepthStencilState(&DepthStencilStateDesc, &pDepthStencilState)) )	return Error(L"Unable to create the"
																											L" depth stencil state");

	// Set the disabled depth stencil state
	DepthStencilStateDesc.DepthEnable		= false;

	if ( FAILED(pDevice->CreateDepthStencilState(&DepthStencilStateDesc, &pDepthStencilStateDisabled)) )	return Error(L"Unable to create the"
																															L" 2D depth stencil"
																															L" state");
	// Set our render targets
	pDeviceContext->OMSetRenderTargets(1, &pRenderTarget, pDepthStencilView);

	return true;
}

bool CGraphics::ResizeBuffers(HWND hMainWindow)
{
	pGetDepthStencil()->Release();
	pGetDepthStencilState()->Release();
	pGetDisabledDepthStencilState()->Release();
	pGetDepthStencilView()->Release();
	pGetRenderTarget()->Release();

	RECT rect;

	GetClientRect(hMainWindow, &rect);

	pGetSwapChain()->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D	*pBackBuffer;
	// Call GetBuffer, if fails, return false
	if ( FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer)) )		return Error(L"Unable to get buffer from swap chain");

	if ( FAILED(pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTarget)) )		return Error(L"Unable to create render target");

	pBackBuffer->Release();

	D3D11_TEXTURE2D_DESC	DepthDesc;

	DepthDesc.Width				 = rect.right;
	DepthDesc.Height			 = rect.bottom;
	DepthDesc.MipLevels			 = 1;
	DepthDesc.ArraySize			 = 1;
	DepthDesc.Format			 = DXGI_FORMAT_D32_FLOAT;
	DepthDesc.SampleDesc.Count	 = 1;
	DepthDesc.SampleDesc.Quality = 0;
	DepthDesc.Usage				 = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags			 = D3D11_BIND_DEPTH_STENCIL;
	DepthDesc.CPUAccessFlags	 = 0;
	DepthDesc.MiscFlags			 = 0;

	if ( FAILED(pDevice->CreateTexture2D(&DepthDesc, NULL, &pDepthStencil)) )	return Error(L"Unable to create "
																							  L"the depth buffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC	DepthViewDesc;

	DepthViewDesc.Format			 = DepthDesc.Format;
	DepthViewDesc.ViewDimension		 = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthViewDesc.Texture2D.MipSlice = 0;

	if ( FAILED(pDevice->CreateDepthStencilView(pDepthStencil, &DepthViewDesc, &pDepthStencilView)) )	 return Error(L"Unable to create "
																													   L"the depth stencil view");
	
	D3D11_DEPTH_STENCIL_DESC	DepthStencilStateDesc;

	ZeroMemory(&DepthStencilStateDesc, sizeof(DepthStencilStateDesc));

	DepthStencilStateDesc.DepthEnable		= true;
	DepthStencilStateDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilStateDesc.DepthFunc			= D3D11_COMPARISON_LESS;

	DepthStencilStateDesc.StencilEnable		= true;
	DepthStencilStateDesc.StencilReadMask	= 0xFF;
	DepthStencilStateDesc.StencilWriteMask	= 0xFF;

	DepthStencilStateDesc.FrontFace.StencilFailOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.FrontFace.StencilDepthFailOp	=   D3D11_STENCIL_OP_INCR;
	DepthStencilStateDesc.FrontFace.StencilPassOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.FrontFace.StencilFunc			=	D3D11_COMPARISON_ALWAYS;

	DepthStencilStateDesc.BackFace.StencilFailOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.BackFace.StencilDepthFailOp	=	D3D11_STENCIL_OP_DECR;
	DepthStencilStateDesc.BackFace.StencilPassOp		=	D3D11_STENCIL_OP_KEEP;
	DepthStencilStateDesc.BackFace.StencilFunc			=	D3D11_COMPARISON_ALWAYS;

	if ( FAILED(pDevice->CreateDepthStencilState(&DepthStencilStateDesc, &pDepthStencilState)) )	return Error(L"Unable to create the"
																											L" depth stencil state");

	pDeviceContext->OMSetDepthStencilState(pDepthStencilState, 1);

	// Set the disabled depth stencil state
	DepthStencilStateDesc.DepthEnable		= false;

	if ( FAILED(pDevice->CreateDepthStencilState(&DepthStencilStateDesc, &pDepthStencilStateDisabled)) )	return Error(L"Unable to create the"
																															L" 2D depth stencil"
																															L" state");
	// Set our render targets
	pDeviceContext->OMSetRenderTargets(1, &pRenderTarget, pDepthStencilView);

	CreatesViewports(rect.right, rect.bottom);

	// Check if we are on fullscreen
	if (rect.right == GetSystemMetrics(SM_CXSCREEN))
	{
		if (rect.bottom == GetSystemMetrics(SM_CYSCREEN))
		{
			::gFullScreen = true;
		}
	}
	else
	{
		::gFullScreen = false;
	}

	return true;
}

void CGraphics::CreatesViewports(int width, int height)
{
	D3D11_VIEWPORT viewport;
	
	// As we do not leave any variable without a value, we do not need to fill al the struct with zeroes
	viewport.Height					= static_cast<float>(height);			// height of the window
	viewport.Width					= static_cast<float>(width);			// width of the window
	viewport.MaxDepth				= 1.0f;				// maximum depth of our viewport
	viewport.MinDepth				= 0.0f;				// minimum depth of our viewport
	viewport.TopLeftX				= 0;				// set x and y top left positions
	viewport.TopLeftY				= 0;
	
	// Set the viewport
	pDeviceContext->RSSetViewports(1, &viewport);

	return;
}

bool CGraphics::SetsBuffers(unsigned int index)
{
	// Calculate the number of vertices in the array
	models[index]->SetNumVertices( importer->uiGetsNumVertices() );

	// Create another array for holding our indexed buffers

	// Calculate the number of indices in the array)
	models[index]->SetNumIndices( importer->uiGetsNumIndices() );
	// Create a buffer description
	D3D11_BUFFER_DESC bufferDesc;

	// Here we are describing our vertex buffer, which will include all the vertices of the cube
	bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;							// Vertex buffer
	bufferDesc.ByteWidth			= sizeof(SimpleVertex) * models[index]->GetNumVertices();		// Size of the buffer
	bufferDesc.CPUAccessFlags		= 0;												// No CPU access
	bufferDesc.MiscFlags			= 0;												// No special flags
	bufferDesc.Usage				= D3D11_USAGE_DEFAULT;								// Default usage	
	bufferDesc.StructureByteStride	= 0;

	// Create another buffer for holding our indices
	D3D11_BUFFER_DESC indicesBufferDesc;
		
	indicesBufferDesc.BindFlags				 = D3D11_BIND_INDEX_BUFFER;					// Index buffer
	indicesBufferDesc.ByteWidth				 = sizeof(DWORD) * models[index]->GetNumIndices();		// Size of the buffer
	indicesBufferDesc.CPUAccessFlags		 = 0;										// No CPU access
	indicesBufferDesc.MiscFlags				 = 0;										// No special flags
	indicesBufferDesc.Usage					 = D3D11_USAGE_DEFAULT;						// Default usage
	indicesBufferDesc.StructureByteStride	 = 0; 


	// Create 2 subresources which will include the buffers in memory
	// One subresource is for the index buffer
	D3D11_SUBRESOURCE_DATA subDataindices;
	// The other, for the vertex buffer
	D3D11_SUBRESOURCE_DATA subData;

	subData.pSysMem		= importer->pVBuffer;

	subDataindices.pSysMem	= importer->pIBuffer;

	// Create index buffer
	if ( FAILED(pDevice->CreateBuffer(&indicesBufferDesc, &subDataindices, models[index]->ppGetIndexBuffer())) )	return Error(L"Unable to create index buffer");

	// Create vertex buffer
	if ( FAILED(pDevice->CreateBuffer(&bufferDesc, &subData, models[index]->ppGetVertexBuffer())) )	return Error(L"Unable to create vertex buffer");

	return true;
}

bool CGraphics::ImportsTextures(unsigned int index)
{
	vector <string> Texture_names =  importer->GetModelTextures();

	unsigned int TexturesCount = Texture_names.size();

	models[index]->pGetMaterial()->pGetTextureNames()->resize(0);
	models[index]->pGetMaterial()->pGetTextures()->resize(TexturesCount);
	
	for (unsigned int i = 0; i < TexturesCount; i++)
	{ 
		models[index]->pGetMaterial()->pGetTextureNames()->push_back(Texture_names[i]);
	}

	for (unsigned int i = 0; i < TexturesCount; i++)
	{
		string name   = models[index]->pGetMaterial()->GetTextureNameByIndex(i);

		LPWSTR _name  = 0;

		::StrToLPWSTR(name, _name);

		if ( FAILED(D3DX11CreateShaderResourceViewFromFile(pDevice, _name, NULL, NULL,
																models[index]->pGetMaterial()->ppGetTextureByIndex(i),
																NULL)) )
		{
			return false;
		}
		// Do the clean up
		
		models[index]->pGetMaterial()->pGetTextureNames()->resize(0);
	}


	importer->CleanTextures();
	return true;
}

void CGraphics::SetsScene(int width, int height)
{

	// Set our primitive topology to be a trianglelist
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Call our InitCamera function
	camera->InitCamera(width, height, ::gCamNear, ::gCamDepth);
    // Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH( &camera->projectionMatrix, static_cast<float>(D3DX_PI * 0.5f), static_cast<float>(width) / static_cast<float>(height), ::gCamNear, ::gCamDepth );

	//D3DXMatrixOrthoLH( &camera->projectionMatrix,  static_cast<float>(D3DX_PI * 0.5f), static_cast<float>(width / height), gCamNear, gCamDepth );

	return;
}

