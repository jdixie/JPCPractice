#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <assert.h>
#include <fstream>
#include <vector>
#include "constants.h"
#include "gameError.h"
#include "lighting.h"
#include "TextureVertexShader.h"
#include "TexturePixelShader.h"
#include "ColorVertexShader.h"
#include "ColorPixelShader.h"
#include "WICTextureLoader.h"
#include "SkinnedData.h"
#include "camera.h"
#include "Matrix4.h"

/*
Shader Variables:
tVShader - basic texture vertex shader
tPShader - basic texture pixel shader
cVShader - basic color vertex shader
cPShader - basic color pixel shader
*/

#define MAX_LIGHTS 8

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT2 blendWeights[4]; //bone index, weight pairs
};

/*__declspec(align(16)) struct InstanceData
{
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX InverseTransposeWorldMatrix;
};*/

struct ColorVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
};

struct BoundingBox
{
	std::vector<float> point;
};

struct Material
{
	Material()
	: Emissive(0.0f, 0.0f, 0.0f, 1.0f)
	, Ambient(0.1f, 0.1f, 0.1f, 1.0f)
	, Diffuse(1.0f, 1.0f, 1.0f, 1.0f)
	, Specular(1.0f, 1.0f, 1.0f, 1.0f)
	, SpecularPower(128.0f)
	, UseTexture(false)
	{}

	DirectX::XMFLOAT4   Emissive;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4   Ambient;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4   Diffuse;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4   Specular;
	//----------------------------------- (16 byte boundary)
	float               SpecularPower;
	// Add some padding complete the 16 byte boundary.
	int                 UseTexture;
	// Add some padding to complete the 16 byte boundary.
	float                 Padding[2];
	//----------------------------------- (16 byte boundary)
}; // Total:                                80 bytes (5 * 16)

struct MaterialProperties
{
	Material Mat;
};

enum LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};

struct Light
{
	Light()
	: Position(0.0f, 0.0f, 0.0f, 1.0f)
	, Direction(0.0f, 0.0f, 1.0f, 0.0f)
	, Color(1.0f, 1.0f, 1.0f, 1.0f)
	, SpotAngle(DirectX::XM_PIDIV2)
	, ConstantAttenuation(1.0f)
	, LinearAttenuation(0.0f)
	, QuadraticAttenuation(0.0f)
	, LightType(DirectionalLight)
	, Enabled(0)
	{}

	DirectX::XMFLOAT4    Position;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4    Direction;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4    Color;
	//----------------------------------- (16 byte boundary)
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	//----------------------------------- (16 byte boundary)
	int         LightType;
	int         Enabled;
	// Add some padding to make this struct size a multiple of 16 bytes.
	int         Padding[2];
	//----------------------------------- (16 byte boundary)
};  // Total:                              80 bytes ( 5 * 16 )

struct LightProperties
{
	LightProperties()
	: EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
	, GlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f)
	{}

	DirectX::XMFLOAT4   EyePosition;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4   GlobalAmbient;
	//----------------------------------- (16 byte boundary)
	Light               Lights[MAX_LIGHTS]; // 80 * 8 bytes
};  // Total:                                  672 bytes (42 * 16)

struct Particle
{
	DirectX::XMFLOAT3 initialPos;
	DirectX::XMFLOAT3 initialVel;
	DirectX::XMFLOAT2 size;
	float age;
	UINT type;
};

struct Bone
{
	int id;
	//necessary?
	float length;
	Bone *parent, *child, *sibling;
	//for transforming
	XMFLOAT4 rotation;
	XMFLOAT3 translation, scale;
	XMMATRIX localTransforms, skinningMatrix;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<ColorVertex> colorVertices;
	std::vector<unsigned int> indices;
	std::vector<MaterialProperties> materials;
	std::vector<unsigned int> attributes;
	std::vector<ID3D11ShaderResourceView*> textureMapSRV;
	SkinnedData skinnedData;
	std::vector<Bone> bones;
};

struct ImageInfo
{
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float scale;
	Matrix4 rotation;
	UINT totalVertices;
	UINT totalIndices;
	UINT totalMaterials;
	UINT totalBones;
	UINT totalAnimations;
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	float blendFactors[4]; //for transparency blending, 0-1 per term
	float color[4]; //for non textured color drawing
	MeshData *mesh;
};

//shader info
enum ColorConstantBuffer
{
	CCB_Application,
	CCB_Frame,
	CCB_Object,
	NumColorConstantBuffers
};
struct PerFrameConstantBufferData
{
	DirectX::XMMATRIX ViewProjectionMatrix;
};

__declspec(align(16)) struct PerObjectConstantBufferData
{
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX InverseTransposeWorldMatrix;
	DirectX::XMMATRIX WorldViewProjectionMatrix;
};


class Graphics
{
private:
	HRESULT hr;									//std hresult
	ID3D11Device *dxDevice;						//dx11 device
	ID3D11DeviceContext *dxDeviceContext;		//dx11 context.  can be immediate for single thread
	D3D_FEATURE_LEVEL featureLevel;				//dx11 feature level
	IDXGISwapChain *swapChain;					//dxgi swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;			//dx11 swap chain descriptors
	UINT dx4xMsaaQuality;						//aa quality of hardware
	ID3D11RenderTargetView *renderTargetview;	//ptr to the render target
	D3D11_TEXTURE2D_DESC depthStencilDesc;		//depth and stencil buffer descriptors
	ID3D11Texture2D *depthStencilBuffer;
	ID3D11DepthStencilView *depthStencilView;
	ID3D11DepthStencilState *depthStencilState;
	D3D11_VIEWPORT viewport;
	ID3D11RasterizerState *rasterizerState;		//rasterization state pointer

	//shader data
	ID3D11VertexShader *textureVertexShader;
	ID3D11PixelShader *texturePixelShader;
	ID3D11SamplerState *textureSamplerState;
	ID3D11VertexShader *colorVertexShader;
	ID3D11PixelShader *colorPixelShader;
	ID3D11Buffer *colorConstantBuffers[NumColorConstantBuffers];
	ID3D11Buffer *textureConstantBuffer;
	ID3D11Buffer *materialConstantBuffer;
	ID3D11Buffer *lightingConstantBuffer;

	ID3D11InputLayout *textureVertexInputLayout;//input layout for vertex with texture
	ID3D11InputLayout *colorVertexInputLayout;	//input layout for vertex with color

	PerObjectConstantBufferData objectBufferData;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projMatrix;

/*	ID3DX11EffectShaderResourceVariable *fxTextureMap;	//for texturing*/
	//move this to imageinfo later, here for testing
	DirectX::XMMATRIX texTransformMatrix;

	//blending, can be done in effect file instead. we'll see
	ID3D11BlendState *transparentBS;

	//lighting
	//DirectionalLight dirLight;
	//SpotLight spotLight;
	//Material material;
	//DirectX::XMFLOAT3 eyePosW;

	//lighting testing
	LightProperties lightTests;

	//options
	bool vSync = true;

	float width;					//screen width
	float height;					//screen height
	bool enable4xMsaa;

	//get profile for shader type
	template< class ShaderClass >
	std::string GetLatestProfile();

	//vertex profile
	template<>
	std::string GetLatestProfile<ID3D11VertexShader>()
	{
		assert(dxDevice);

		// Query the current feature level:
		D3D_FEATURE_LEVEL featureLevel = dxDevice->GetFeatureLevel();

		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0:
		{
									   return "vs_5_0";
		}
			break;
		case D3D_FEATURE_LEVEL_10_1:
		{
									   return "vs_4_1";
		}
			break;
		case D3D_FEATURE_LEVEL_10_0:
		{
									   return "vs_4_0";
		}
			break;
		case D3D_FEATURE_LEVEL_9_3:
		{
									  return "vs_4_0_level_9_3";
		}
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
		{
									  return "vs_4_0_level_9_1";
		}
			break;
		} // switch( featureLevel )

		return "";
	}

	//pixel profile
	template<>
	std::string GetLatestProfile<ID3D11PixelShader>()
	{
		assert(dxDevice);

		// Query the current feature level:
		D3D_FEATURE_LEVEL featureLevel = dxDevice->GetFeatureLevel();
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0:
		{
									   return "ps_5_0";
		}
			break;
		case D3D_FEATURE_LEVEL_10_1:
		{
									   return "ps_4_1";
		}
			break;
		case D3D_FEATURE_LEVEL_10_0:
		{
									   return "ps_4_0";
		}
			break;
		case D3D_FEATURE_LEVEL_9_3:
		{
									  return "ps_4_0_level_9_3";
		}
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
		{
									  return "ps_4_0_level_9_1";
		}
			break;
		}
		return "";
	}

	template< class ShaderClass >
	ShaderClass *createShader(ID3DBlob *shaderBlob, ID3D11ClassLinkage *classLinkage);

	template<>
	ID3D11VertexShader *createShader<ID3D11VertexShader>(ID3DBlob *shaderBlob, ID3D11ClassLinkage *classLinkage)
	{
		assert(dxDevice);
		assert(shaderBlob);

		ID3D11VertexShader *shader = 0;
		dxDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), classLinkage, &shader);

		return shader;
	}

	template<>
	ID3D11PixelShader *createShader<ID3D11PixelShader>(ID3DBlob *shaderBlob, ID3D11ClassLinkage *classLinkage)
	{
		assert(dxDevice);
		assert(shaderBlob);

		ID3D11PixelShader *shader = 0;
		dxDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), classLinkage, &shader);

		return shader;
	}

	template< class ShaderClass >
	ShaderClass *loadShader(const std::wstring &fileName, const std::string &entryPoint, const std::string &_profile)
	{
		ID3DBlob *shaderBlob = 0;
		ID3DBlob *errorBlob = 0;
		ShaderClass *shader = 0;

		std::string profile = _profile;
		if (profile == "latest")
		{
			profile = GetLatestProfile<ShaderClass>();
		}

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#if _DEBUG
			flags |= D3DCOMPILE_DEBUG;
		#endif

		HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), profile.c_str(),
			flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				std::string errorMessage = (char*)errorBlob->GetBufferPointer();
				OutputDebugStringA(errorMessage.c_str());

				SafeRelease(shaderBlob);
				SafeRelease(errorBlob);
			}

			return false;
		}

		shader = createShader<ShaderClass>(shaderBlob, 0);

		safeRelease(shaderBlob);
		safeRelease(errorBlob);

		return shader;
	}


public:
	//camera class - located here to provide the ability to the camera to create world transforms in addition
	//to its view transforms. In this way I intend the camera class to control how the scene is viewed at all
	//times
	Camera camera;
	//constructor/destructor
	Graphics();
	virtual ~Graphics();

	virtual ID3D11Device *getDevice() { return dxDevice; }
	//initialize graphics device and properties
	virtual void initialize(HWND hwnd, float w, float h, bool full);
	virtual void beginRender(LightProperties *lp);
	virtual void endRender();

	virtual void setNoCullRastState();
	virtual void setProjMatrix();
	//virtual void setViewMatrix(float x, float y, float z);
	//virtual void setViewMatrix(DirectX::XMVECTOR pos, DirectX::XMVECTOR target, DirectX::XMVECTOR up);
	virtual void setViewMatrix();

	virtual ID3D11ShaderResourceView *createTextureSRV(const wchar_t *file)
	{
		ID3D11ShaderResourceView *texSRV;
		hr = CreateWICTextureFromFile(dxDevice, dxDeviceContext, file, NULL, &texSRV, NULL);
		//hr = D3DX11CreateShaderResourceViewFromFile(dxDevice, file, 0, 0, &texSRV, 0);
		if (FAILED(hr))
			throw(GameError(gameErrorNS::FATAL_ERROR, L"Error creating texture resource."));
		return texSRV;
	}

	virtual ID3D11Buffer *createVertexColorBuffer(std::vector<Vertex>* vertices, UINT totalVertices)
	{
		ID3D11Buffer *vtb;
		
		D3D11_BUFFER_DESC vbdt;
		vbdt.Usage = D3D11_USAGE_IMMUTABLE;
		vbdt.ByteWidth = sizeof(ColorVertex)* totalVertices;
		vbdt.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbdt.CPUAccessFlags = 0;
		vbdt.MiscFlags = 0;
		vbdt.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srdt;
		srdt.pSysMem = vertices;
		dxDevice->CreateBuffer(&vbdt, &srdt, &vtb);

		return vtb;
	}

	virtual ID3D11Buffer *createVertexTextureBuffer(std::vector<Vertex>* vertices, UINT totalVertices)
	{
		ID3D11Buffer *vtb;

		D3D11_BUFFER_DESC vbdt;
		vbdt.Usage = D3D11_USAGE_IMMUTABLE;
		vbdt.ByteWidth = sizeof(Vertex)* totalVertices;
		vbdt.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbdt.CPUAccessFlags = 0;
		vbdt.MiscFlags = 0;
		vbdt.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srdt;
		srdt.pSysMem = &vertices[0];
		dxDevice->CreateBuffer(&vbdt, &srdt, &vtb);

		return vtb;
	}

	virtual ID3D11Buffer *createIndexBuffer(DWORD indices[], UINT totalIndices)
	{
		ID3D11Buffer *ib;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT)* totalIndices;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srdi;
		srdi.pSysMem = indices;

		dxDevice->CreateBuffer(&ibd, &srdi, &ib);

		return ib;
	}

	virtual ID3D11Buffer *createIndexBuffer(std::vector<DWORD>* indices, UINT totalIndices)
	{
		ID3D11Buffer *ib;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT)* totalIndices;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srdi;
		srdi.pSysMem = &indices[0];

		dxDevice->CreateBuffer(&ibd, &srdi, &ib);

		return ib;
	}

	virtual void drawTextured(ImageInfo *image);
	virtual void drawColored(ImageInfo *image);

	//blending
	virtual void setBlendStateNotTransparent() { dxDeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff); }
	virtual void setBlendStateTransparent(float blendFactors[]) { dxDeviceContext->OMSetBlendState(transparentBS, blendFactors, 0xffffffff); }

};

#endif