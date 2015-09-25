#include "Graphics.h"

Graphics::Graphics()
{
	dxDevice = 0;
	dxDeviceContext = 0;
	swapChain = 0;
	renderTargetview = 0;
	depthStencilBuffer = 0;
	depthStencilState = 0;
	depthStencilView = 0;
	rasterizerState = 0;
	colorVertexShader = 0;
	colorPixelShader = 0;
	enable4xMsaa = true;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
}

Graphics::~Graphics()
{
	//unloadables?
	safeRelease(colorConstantBuffers[CCB_Application]);
	safeRelease(colorConstantBuffers[CCB_Frame]);
	safeRelease(colorConstantBuffers[CCB_Object]);
	safeRelease(textureVertexInputLayout);
	safeRelease(colorVertexInputLayout);
	safeRelease(textureVertexShader);
	safeRelease(texturePixelShader);
	safeRelease(colorVertexShader);
	safeRelease(colorPixelShader);

	safeRelease(rasterizerState);
	safeRelease(renderTargetview);
	safeRelease(depthStencilView);
	safeRelease(depthStencilBuffer);
	safeRelease(depthStencilState);
	safeRelease(swapChain);
	safeRelease(dxDeviceContext);
	safeRelease(dxDevice);

	safeRelease(adapter);
	safeRelease(d3d101Device);
	safeRelease(keyedMutex11);
	safeRelease(keyedMutex10);
	safeRelease(D2DRenderTarget);
	safeRelease(Brush);
	safeRelease(BackBuffer11);
	safeRelease(sharedTex11);
	safeRelease(d2dVertBuffer);
	safeRelease(d2dIndexBuffer);
	safeRelease(d2dTexture);
	safeRelease(DWriteFactory);
	safeRelease(TextFormat);
	safeRelease(Transparency);
}

//initialize graphics device and properties
void Graphics::initialize(HWND hwnd, float w, float h, bool full)
{
	width = w;
	height = h;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;;

	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION, &dxDevice, &featureLevel, &dxDeviceContext);
//	hr = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
//		NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &dxDevice, NULL, &dxDeviceContext);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating Direct3D device."));
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Device does not support DirectX 11."));
	}

	//get aa quality
	dxDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &dx4xMsaaQuality);
	assert(dx4xMsaaQuality > 0 );

	//setup swap chain description
	SecureZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//msaa info
	if (enable4xMsaa)
	{
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = dx4xMsaaQuality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = !full;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//create device and swapchain
	
	//feature levels accepted
	/*D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT createDeviceFlags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &dxDevice, &featureLevel, &dxDeviceContext);

	//make sure device was created successfully and supports dx11
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating Direct3D device."));
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Device does not support DirectX 11."));*/
	IDXGIDevice *dxgiDevice = 0;				//dxgi device
	//IDXGIAdapter *dxgiAdapter = 0;				//dxgi adapter
	IDXGIFactory *dxgiFactory = 0;				//dxgi factory

	hr = dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error querying dxgi device."));
	//hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error getting dxgi device parent."));
	//hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error getting dxgi adapter parent."));
	hr = dxgiFactory->CreateSwapChain(dxDevice, &swapChainDesc, &swapChain);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating swap chain."));
	dxgiFactory->Release();
	//dxgiAdapter->Release();
	dxgiDevice->Release();

	//grab our render target info
	ID3D11Texture2D *backBuffer;				//ptr to the back buffer

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating back buffer."));
	hr = dxDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetview);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating render target view."));
	safeRelease(backBuffer);

	//set depth and stencil info
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//msaa info must match swap chain info
	depthStencilDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count;
	depthStencilDesc.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	//create depth and stencil buffer, view and state
	hr = dxDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating depth stencil buffer."));
	}
	hr = dxDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating depth stencil view."));
	}
	hr = dxDevice->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating depth stencil state."));
	}

	//bind our views to the output merger
	dxDeviceContext->OMSetRenderTargets(1, &renderTargetview, depthStencilView);
	dxDeviceContext->OMSetDepthStencilState(depthStencilState, 1);

	//set up constant buffers for shader interaction
	D3D11_BUFFER_DESC colorConstantBufferDesc;
	ZeroMemory(&colorConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	colorConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorConstantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
	colorConstantBufferDesc.CPUAccessFlags = 0;
	colorConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = dxDevice->CreateBuffer(&colorConstantBufferDesc, nullptr, &colorConstantBuffers[CCB_Application]);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating color constant buffer PerApplication."));
	}
	hr = dxDevice->CreateBuffer(&colorConstantBufferDesc, nullptr, &colorConstantBuffers[CCB_Frame]);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating color constant buffer PerFrame."));
	}
	hr = dxDevice->CreateBuffer(&colorConstantBufferDesc, nullptr, &colorConstantBuffers[CCB_Object]);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating color constant buffer PerObject."));
	}

	D3D11_BUFFER_DESC textureConstantBufferDesc;
	ZeroMemory(&textureConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	textureConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	textureConstantBufferDesc.ByteWidth = sizeof(PerObjectConstantBufferData);
	textureConstantBufferDesc.CPUAccessFlags = 0;
	textureConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = dxDevice->CreateBuffer(&textureConstantBufferDesc, 0, &textureConstantBuffer);

	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating texture constant buffer."));
	}

	textureConstantBufferDesc.ByteWidth = sizeof(MaterialProperties);
	hr = dxDevice->CreateBuffer(&textureConstantBufferDesc, 0, &materialConstantBuffer);

	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating material constant buffer."));
	}

	textureConstantBufferDesc.ByteWidth = sizeof(LightProperties);
	hr = dxDevice->CreateBuffer(&textureConstantBufferDesc, 0, &lightingConstantBuffer);

	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating lighting constant buffer."));
	}


	//create the shaders
	hr = dxDevice->CreateVertexShader(tVShader, sizeof(tVShader), nullptr, &textureVertexShader);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating textured vertex shader."));
	}

	hr = dxDevice->CreatePixelShader(tPShader, sizeof(tPShader), nullptr, &texturePixelShader);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating textured pixel shader."));
	}

	hr = dxDevice->CreateVertexShader(cVShader, sizeof(cVShader), nullptr, &colorVertexShader);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating colored vertex shader."));
	}

	hr = dxDevice->CreatePixelShader(cPShader, sizeof(cPShader), nullptr, &colorPixelShader);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating colored pixel shader."));
	}
	

	//setup vertex interface
	//setup input layout descriptions 0-12-24
	D3D11_INPUT_ELEMENT_DESC TextureVertexDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = dxDevice->CreateInputLayout(TextureVertexDesc, _countof(TextureVertexDesc), tVShader, sizeof(tVShader), &textureVertexInputLayout);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating vertex input layout."));
	}

	D3D11_INPUT_ELEMENT_DESC  ColorVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(ColorVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(ColorVertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = dxDevice->CreateInputLayout(ColorVertexDesc, _countof(ColorVertexDesc), cVShader, sizeof(cVShader), &colorVertexInputLayout);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating vertex input layout."));
	}

	dxDeviceContext->IASetInputLayout(textureVertexInputLayout);

	//setup no cull rasterization state
	setNoCullRastState();

	//set up viewport
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	dxDeviceContext->RSSetViewports(1, &viewport);

	//setup wvp matrices
	DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
	objectBufferData.WorldMatrix = identityMatrix;
	objectBufferData.InverseTransposeWorldMatrix = identityMatrix;
	objectBufferData.WorldViewProjectionMatrix = identityMatrix;
	viewMatrix = identityMatrix;
	projMatrix = identityMatrix;

	setProjMatrix();

	//setup texture sampler
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = dxDevice->CreateSamplerState(&samplerDesc, &textureSamplerState);
	if (FAILED(hr))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating texture sampler."));
	}
	dxDeviceContext->PSSetSamplers(0, 1, &textureSamplerState);

	InitD2D_D3D101_DWrite();

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	dxDevice->CreateBlendState(&blendDesc, &Transparency);
	textImage.mesh = new MeshData;
	InitD2DScreenTexture();
	textImage.mesh->materials.at(0).Mat.Ambient = DirectX::XMFLOAT4(1, 1, 1, 1);
	textImage.rotation.makeRotate(180, 1, 0, 0);
}

void Graphics::setNoCullRastState()
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable = true;

	dxDevice->CreateRasterizerState(&rastDesc, &rasterizerState);
	dxDeviceContext->RSSetState(rasterizerState);
}

void Graphics::setProjMatrix()
{
	projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), width / height, 1.0f, 10000.0f);
	dxDeviceContext->UpdateSubresource(colorConstantBuffers[CCB_Application], 0, 0, &projMatrix, 0, 0);
}

//first/third person view matrix
//void Graphics::setViewMatrix(DirectX::XMVECTOR pos, DirectX::XMVECTOR target, DirectX::XMVECTOR up)
//{
//	viewMatrix = DirectX::XMMatrixLookAtLH(pos, target, up);
//	dxDeviceContext->UpdateSubresource(colorConstantBuffers[CCB_Frame], 0, 0, &viewMatrix, 0, 0);
//}
void Graphics::setViewMatrix()
{
	viewMatrix = DirectX::XMMatrixLookAtLH(camera.getPosition(), camera.getTarget(), camera.getUp());
	dxDeviceContext->UpdateSubresource(colorConstantBuffers[CCB_Frame], 0, 0, &viewMatrix, 0, 0);
}

void Graphics::beginRender(LightProperties *lp)
{
	//clear the back buffer, set back color to black
    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    dxDeviceContext->ClearRenderTargetView(renderTargetview, color);
	dxDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dxDeviceContext->UpdateSubresource(lightingConstantBuffer, 0, nullptr, lp, 0, 0);
}

void Graphics::drawTextured(ImageInfo *image)
{
	DirectX::XMMATRIX i = DirectX::XMMatrixIdentity();
	

	//set our topology
	dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //strip?

	//ready the vertices and transform matrix
	DirectX::XMMATRIX scale
		(
			image->scale, 0, 0, 0,
			0, image->scale, 0, 0,
			0, 0, image->scale, 0,
			0, 0, 0, 1
		);

	DirectX::XMMATRIX rotation
		(
			image->rotation.m00, image->rotation.m01, image->rotation.m02, image->rotation.m03,
			image->rotation.m10, image->rotation.m11, image->rotation.m12, image->rotation.m13,
			image->rotation.m20, image->rotation.m21, image->rotation.m22, image->rotation.m23,
			image->rotation.m30, image->rotation.m31, image->rotation.m23, image->rotation.m33
		);

	DirectX::XMMATRIX translation
		(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			image->x, image->y, image->z, 1
		);
	//DirectX::XMMATRIX world = scale*camera.getWorldRotationMatrix()*translation;
	DirectX::XMMATRIX world = scale*rotation*translation;
	objectBufferData.WorldMatrix = world;

	DirectX::XMVECTOR worldDet = DirectX::XMMatrixDeterminant(world);
	DirectX::XMMATRIX worldInv = DirectX::XMMatrixInverse(&worldDet, world);
	//DirectX::XMMATRIX worldInvTrans = DirectX::XMMatrixTranspose(worldInv);
	objectBufferData.InverseTransposeWorldMatrix = DirectX::XMMatrixTranspose(worldInv);

	DirectX::XMMATRIX wvp = (DirectX::XMMATRIX)world * (DirectX::XMMATRIX)viewMatrix * (DirectX::XMMATRIX)projMatrix;
	//objectBufferData.WorldViewProjectionMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, viewMatrix), projMatrix);
	objectBufferData.WorldViewProjectionMatrix = wvp;
	dxDeviceContext->UpdateSubresource(textureConstantBuffer, 0, 0, &objectBufferData, 0, 0);

	//ready the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dxDeviceContext->IASetVertexBuffers(
		0,                //the first input slot for binding
		1,                //the number of buffers in the array
		&image->vertexBuffer,			  //the array of vertex buffers
		&stride,          //array of stride values, one for each buffer
		&offset);         //array of offset values, one for each buffer
	dxDeviceContext->IASetIndexBuffer(image->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dxDeviceContext->IASetInputLayout(textureVertexInputLayout);
	//bind shaders
	dxDeviceContext->VSSetShader(textureVertexShader, 0, 0);
	dxDeviceContext->VSSetConstantBuffers(0, 1, &textureConstantBuffer);
	dxDeviceContext->PSSetShader(texturePixelShader, 0, 0);
	dxDeviceContext->UpdateSubresource(materialConstantBuffer, 0, 0, &image->mesh->materials.at(0), 0, 0);
	ID3D11Buffer *pixelShaderConstantBuffers[2] = { materialConstantBuffer, lightingConstantBuffer };
	dxDeviceContext->PSSetConstantBuffers(0, 2, pixelShaderConstantBuffers);

	dxDeviceContext->PSSetSamplers(0, 1, &textureSamplerState);
	dxDeviceContext->PSSetShaderResources(0, 1, &image->mesh->textureMapSRV.at(0));

	dxDeviceContext->OMSetRenderTargets(1, &renderTargetview, depthStencilView);
	dxDeviceContext->OMSetDepthStencilState(depthStencilState, 1);
	//draw
	dxDeviceContext->DrawIndexed(image->totalIndices, 0, 0);
}

/*void Graphics::drawColored(ImageInfo *image)
{
	DirectX::XMMATRIX i = DirectX::XMMatrixIdentity();
	//get texture ready


	//set our topology
	dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //strip?

	//ready the vertices and transform matrix
	DirectX::XMMATRIX scale
		(
		image->scale, 0, 0, 0,
		0, image->scale, 0, 0,
		0, 0, image->scale, 0,
		0, 0, 0, 1
		);
	DirectX::XMMATRIX rotation
		(
		image->rotation.m00, image->rotation.m01, image->rotation.m02, image->rotation.m03,
		image->rotation.m10, image->rotation.m11, image->rotation.m12, image->rotation.m13,
		image->rotation.m20, image->rotation.m21, image->rotation.m22, image->rotation.m23,
		image->rotation.m30, image->rotation.m31, image->rotation.m23, image->rotation.m33
		);
	DirectX::XMMATRIX translation
		(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		image->x, image->y, image->z, 1
		);
	//DirectX::XMMATRIX worldMatrix = scale*camera.getWorldRotationMatrix()*translation;
	DirectX::XMMATRIX worldMatrix = scale*rotation*translation;

	dxDeviceContext->UpdateSubresource(colorConstantBuffers[CCB_Object], 0, 0, &worldMatrix, 0, 0);


	//ready the vertex buffer
	UINT stride = sizeof(ColorVertex);
	UINT offset = 0;
	dxDeviceContext->IASetVertexBuffers(
		0,                //the first input slot for binding
		1,                //the number of buffers in the array
		&image->vertexBuffer,			  //the array of vertex buffers
		&stride,          //array of stride values, one for each buffer
		&offset);         //array of offset values, one for each buffer
	dxDeviceContext->IASetIndexBuffer(image->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dxDeviceContext->IASetInputLayout(colorVertexInputLayout);

	//bind shaders
	dxDeviceContext->VSSetShader(colorVertexShader, 0, 0);
	dxDeviceContext->VSSetConstantBuffers(0, 3, colorConstantBuffers);
	dxDeviceContext->PSSetShader(colorPixelShader, 0, 0);

	//draw
	dxDeviceContext->DrawIndexed(image->totalIndices, 0, 0);
}*/

void Graphics::endRender()
{
	//swap backbuffer
	if (vSync)
		swapChain->Present(1, 0);
	else
		swapChain->Present(0, 0);
}

//Creating writing surface, using code from braynzarsoft
bool Graphics::InitD2D_D3D101_DWrite()
{
	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = width;
	sharedTexDesc.Height = height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = dxDevice->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1 *sharedSurface10;

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));

	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory *D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();

	// Create a solid color brush to draw something with		
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &Brush);

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));

	hr = DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		128.0f,
		L"en-us",
		&TextFormat
		);

	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}

void Graphics::InitD2DScreenTexture()
{
	//Create the vertex buffer
	textImage.totalVertices = 4;
	textImage.totalIndices = 6;
	textImage.x = 0;
	textImage.y = 0;
	textImage.z = 0;

	float l = 100;
	float w = 100;

	textImage.mesh->vertices.resize(4);
	/*textImage.mesh->vertices[0].position = DirectX::XMFLOAT3(-l, 0, -w);
	textImage.mesh->vertices[1].position = DirectX::XMFLOAT3(l, 0, -w);
	textImage.mesh->vertices[2].position = DirectX::XMFLOAT3(l, 0, w);
	textImage.mesh->vertices[3].position = DirectX::XMFLOAT3(-l, 0, w);

	textImage.mesh->vertices[0].normal = DirectX::XMFLOAT3(0, 1, 0);
	textImage.mesh->vertices[1].normal = DirectX::XMFLOAT3(0, 1, 0);
	textImage.mesh->vertices[2].normal = DirectX::XMFLOAT3(0, 1, 0);
	textImage.mesh->vertices[3].normal = DirectX::XMFLOAT3(0, 1, 0);*/
	textImage.mesh->vertices[0].position = DirectX::XMFLOAT3(999, -w, -l);
	textImage.mesh->vertices[1].position = DirectX::XMFLOAT3(999, -w, l);
	textImage.mesh->vertices[2].position = DirectX::XMFLOAT3(999, w, l);
	textImage.mesh->vertices[3].position = DirectX::XMFLOAT3(999, w, -l);

	textImage.mesh->vertices[0].normal = DirectX::XMFLOAT3(-1, 0, 0);
	textImage.mesh->vertices[1].normal = DirectX::XMFLOAT3(-1, 0, 0);
	textImage.mesh->vertices[2].normal = DirectX::XMFLOAT3(-1, 0, 0);
	textImage.mesh->vertices[3].normal = DirectX::XMFLOAT3(-1, 0, 0);

	textImage.mesh->vertices[0].tex = DirectX::XMFLOAT2(0, 0);
	textImage.mesh->vertices[1].tex = DirectX::XMFLOAT2(1, 0);
	textImage.mesh->vertices[2].tex = DirectX::XMFLOAT2(1, 1);
	textImage.mesh->vertices[3].tex = DirectX::XMFLOAT2(0, 1);

	textImage.mesh->indices.resize(6);
	textImage.mesh->indices[0] = 0;
	textImage.mesh->indices[1] = 1;
	textImage.mesh->indices[2] = 2;
	textImage.mesh->indices[3] = 0;
	textImage.mesh->indices[4] = 2;
	textImage.mesh->indices[5] = 3;

	MaterialProperties tempMat;
	tempMat.Mat.UseTexture = true;
	textImage.mesh->materials.push_back(tempMat);

	textImage.totalVertices = textImage.mesh->vertices.size();
	textImage.totalIndices = textImage.mesh->indices.size();

	D3D11_BUFFER_DESC vbdt;
	vbdt.Usage = D3D11_USAGE_IMMUTABLE;
	vbdt.ByteWidth = sizeof(Vertex)* textImage.totalVertices;
	vbdt.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdt.CPUAccessFlags = 0;
	vbdt.MiscFlags = 0;
	vbdt.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdt;
	srdt.pSysMem = &textImage.mesh->vertices[0];
	dxDevice->CreateBuffer(&vbdt, &srdt, &textImage.vertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* textImage.totalIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdi;
	srdi.pSysMem = &textImage.mesh->indices[0];

	dxDevice->CreateBuffer(&ibd, &srdi, &textImage.indexBuffer);
	dxDevice->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);
}

void Graphics::renderText(std::wstring text)//, ImageInfo *image)
{
	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync(0);

	//Use D3D10.1 device
	keyedMutex10->AcquireSync(0, 5);

	//Draw D2D content		
	D2DRenderTarget->BeginDraw();

	//Clear D2D Background
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString;
	printString << text;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	Brush->SetColor(FontColor);

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, 400, 400);

	//Draw the Text
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the blend state for D2D render target texture objects
	//dxDeviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	/*dxDeviceContext->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dxDeviceContext->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);*/

	//ready the vertices and transform matrix
	DirectX::XMMATRIX scale
		(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);

	DirectX::XMMATRIX rotation
		(
		textImage.rotation.m00, textImage.rotation.m01, textImage.rotation.m02, textImage.rotation.m03,
		textImage.rotation.m10, textImage.rotation.m11, textImage.rotation.m12, textImage.rotation.m13,
		textImage.rotation.m20, textImage.rotation.m21, textImage.rotation.m22, textImage.rotation.m23,
		textImage.rotation.m30, textImage.rotation.m31, textImage.rotation.m23, textImage.rotation.m33
		);

	DirectX::XMMATRIX translation
		(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
	//DirectX::XMMATRIX world = scale*camera.getWorldRotationMatrix()*translation;
	DirectX::XMMATRIX world = scale*rotation*translation;
	objectBufferData.WorldMatrix = world;

	DirectX::XMVECTOR worldDet = DirectX::XMMatrixDeterminant(world);
	DirectX::XMMATRIX worldInv = DirectX::XMMatrixInverse(&worldDet, world);
	//DirectX::XMMATRIX worldInvTrans = DirectX::XMMatrixTranspose(worldInv);
	objectBufferData.InverseTransposeWorldMatrix = DirectX::XMMatrixTranspose(worldInv);

	DirectX::XMMATRIX wvp = (DirectX::XMMATRIX)world * (DirectX::XMMATRIX)viewMatrix * (DirectX::XMMATRIX)projMatrix;
	//objectBufferData.WorldViewProjectionMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, viewMatrix), projMatrix);
	objectBufferData.WorldViewProjectionMatrix = wvp;
	dxDeviceContext->UpdateSubresource(textureConstantBuffer, 0, 0, &objectBufferData, 0, 0);

	//ready the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dxDeviceContext->IASetVertexBuffers(
		0,                //the first input slot for binding
		1,                //the number of buffers in the array
		&textImage.vertexBuffer,			  //the array of vertex buffers
		&stride,          //array of stride values, one for each buffer
		&offset);         //array of offset values, one for each buffer
	dxDeviceContext->IASetIndexBuffer(textImage.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dxDeviceContext->IASetInputLayout(textureVertexInputLayout);
	//bind shaders
	dxDeviceContext->VSSetShader(textureVertexShader, 0, 0);
	dxDeviceContext->VSSetConstantBuffers(0, 1, &textureConstantBuffer);
	dxDeviceContext->PSSetShader(texturePixelShader, 0, 0);
	dxDeviceContext->UpdateSubresource(materialConstantBuffer, 0, 0, &textImage.mesh->materials.at(0), 0, 0);
	ID3D11Buffer *pixelShaderConstantBuffers[2] = { materialConstantBuffer, lightingConstantBuffer };
	dxDeviceContext->PSSetConstantBuffers(0, 2, pixelShaderConstantBuffers);

	dxDeviceContext->PSSetSamplers(0, 1, &textureSamplerState);
	dxDeviceContext->PSSetShaderResources(0, 1, &d2dTexture);

	dxDeviceContext->OMSetRenderTargets(1, &renderTargetview, depthStencilView);
	dxDeviceContext->OMSetDepthStencilState(depthStencilState, 1);
	//draw
	dxDeviceContext->DrawIndexed(textImage.totalIndices, 0, 0);

	/*WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dxDeviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dxDeviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dxDeviceContext->PSSetShaderResources(0, 1, &d2dTexture);
	dxDeviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

	dxDeviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	dxDeviceContext->DrawIndexed(6, 0, 0);*/

	//set back to opaque
	dxDeviceContext->OMSetBlendState(0, 0, 0xffffffff);
}