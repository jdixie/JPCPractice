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

	UINT createDeviceFlags = 0;

	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION, &dxDevice, &featureLevel, &dxDeviceContext);
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
	IDXGIAdapter *dxgiAdapter = 0;				//dxgi adapter
	IDXGIFactory *dxgiFactory = 0;				//dxgi factory

	hr = dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error querying dxgi device."));
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error getting dxgi device parent."));
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error getting dxgi adapter parent."));
	hr = dxgiFactory->CreateSwapChain(dxDevice, &swapChainDesc, &swapChain);
	if (FAILED(hr))
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating swap chain."));
	dxgiFactory->Release();
	dxgiAdapter->Release();
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

void Graphics::drawColored(ImageInfo *image)
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
}

void Graphics::endRender()
{
	//swap backbuffer
	if (vSync)
		swapChain->Present(1, 0);
	else
		swapChain->Present(0, 0);
}