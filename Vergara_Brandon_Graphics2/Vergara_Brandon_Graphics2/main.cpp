//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <string.h>
#include "defines.h"
#include "DDSTextureLoader.h"
#include <vector>
#include <thread>

using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
// TODO: PART 1 STEP 1b
#include "math.h"
// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "Star_PS.csh"
#include "Star_VS.csh"
#include "OBJ_VS.csh"
#include "OBJ_PS.csh"
#include "Box_VS.csh"
#include "Box_PS.csh"
#include "LightTest_VS.csh"
#include "LightTest_PS.csh"



#define BACKBUFFER_WIDTH	1280
#define BACKBUFFER_HEIGHT	720

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	IDXGISwapChain *swapChain;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	ID3D11RenderTargetView *pView;
	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT viewport2;
	ID3D11DepthStencilView *pDSV;
	ID3D11Texture2D* pDepthStencil = NULL;
	// TODO: PART 2 STEP 2
	ID3D11Buffer *buff;
	ID3D11Buffer *vertBuff;
	ID3D11Buffer *vertBuffSky;
	ID3D11Buffer *vertBuffCube;
	ID3D11Buffer *ibuff;
	ID3D11Buffer *ibuffSky;
	ID3D11Buffer *ibuffCube;
	ID3D11Buffer *vertBuffLTest;
	ID3D11Buffer *ibuffLTest;
	ID3D11Buffer *vertBuffQuad;
	ID3D11Buffer *ibuffQuad;
	UINT numVerts = 0;
	ID3D11InputLayout *layout;
	ID3D11InputLayout *layout2;
	ID3D11InputLayout *layout3;
	ID3D11InputLayout *layout4;
	ID3D11InputLayout *layout5;
	ID3D11ShaderResourceView *pSRV = {};
	ID3D11ShaderResourceView *pSRV2 = {};
	ID3D11SamplerState* samState;
	ID3D11SamplerState* samState2;

	ID3D11RasterizerState* rasState;
	ID3D11RasterizerState* rasState2;
	
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vShade;
	ID3D11VertexShader *vShade2;
	ID3D11VertexShader *vShade3;
	ID3D11VertexShader *vShade4;
	ID3D11VertexShader *vShade5;
	ID3D11PixelShader *pShade;
	ID3D11PixelShader *pShade2;
	ID3D11PixelShader *pShade3;
	ID3D11PixelShader *pShade4;
	ID3D11PixelShader *pShade5;

	//Transparency
	ID3D11BlendState* Trans;
	ID3D11RasterizerState* CCWcull;
	ID3D11RasterizerState* CWcull;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *ncBuff;
	ID3D11Buffer *lightBuffer;
	XTime timer;

	XMMATRIX camera;
	XMMATRIX camera2;
	XMMATRIX objMatrix;
	XMMATRIX starMatrix;
	XMMATRIX boxMatrix;
	XMMATRIX boxMatrix2;
	XMMATRIX boxMatrix3;
	XMMATRIX quadMatrix;

public:

	struct SIMPLE_VERTEX
	{
		//float x, y, z, w;
		XMFLOAT4 pos;
		XMFLOAT4 col;
		XMFLOAT3 norm;
		//float r, g, b, a;
	};

	struct NEW_VERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT3 uvs;
		XMFLOAT3 norms;
	};

	vector<XMFLOAT3> verts;
	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> norms;
	vector<uint> inds;

	struct OBJECT
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		
	};

	struct LIGHT
	{
		XMFLOAT4 color;
		XMFLOAT3 dir;
		float pad;

		XMFLOAT4 color2;
		XMFLOAT3 dir2;
		float lightRadius;
		XMFLOAT3 pos;
		float pad2;

		XMFLOAT4 color3;
		XMFLOAT3 dir3;
		float coneRatio;
		XMFLOAT3 pos2;
		float pad3;

		XMFLOAT4 camPos;
	};

	//
	//struct cbPerFrame
	//{
	//	Light  light;
	//};
	//
	//cbPerFrame constbuffPerFrame;

	OBJECT toObject;
	OBJECT toObject2;
	LIGHT light;

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

bool loadOBJ(const char* path, vector<XMFLOAT3> &out_verts, vector<XMFLOAT2> &out_uvs, vector<XMFLOAT3> &out_norms, vector<uint> &out_inds)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector< XMFLOAT3 > temp_vertices;
	vector< XMFLOAT2 > temp_uvs;
	vector< XMFLOAT3 > temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		return false;
	}
	
	while (true)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader, 128);
		if (res == EOF)
		{
			break;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		uint vertIndex = vertexIndices[i];
		XMFLOAT3 vert = temp_vertices[vertIndex - 1];
		out_verts.push_back(vert);
		out_inds.push_back(i);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = vertexIndices[i];
		XMFLOAT2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normIndex = vertexIndices[i];
		XMFLOAT3 norm = temp_normals[normIndex - 1];
		out_norms.push_back(norm);
	}
	

	return true;
}

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	
	RegisterClassEx(&wndClass);
	
	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);
	
	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = window;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// TODO: PART 1 STEP 3b
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &scd, &swapChain, &device, NULL, &context);
	// TODO: PART 1 STEP 4
	ID3D11Texture2D *backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	
	device->CreateRenderTargetView(backBuffer, NULL, &pView);
	backBuffer->Release();
	// TODO: PART 1 STEP 5
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.Width = BACKBUFFER_WIDTH/2;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	ZeroMemory(&viewport2, sizeof(D3D11_VIEWPORT));
	viewport2.Height = BACKBUFFER_HEIGHT;
	viewport2.Width = BACKBUFFER_WIDTH/2;
	viewport2.TopLeftX = BACKBUFFER_WIDTH / 2;
	viewport2.TopLeftY = 0;
	viewport2.MinDepth = 0;
	viewport2.MaxDepth = 1;

#pragma region Star
	SIMPLE_VERTEX star[12];


	for (int i = 0; i < 12; ++i)
	{
		if (i == 0 || i == 11)
		{
			star[i].pos.x = 0.0f;
			star[i].pos.y = 0.0f;

			if (i == 0)
				star[i].pos.z = -0.2f;
			else if (i == 11)
				star[i].pos.z = 0.2f;
			star[i].pos.w = 1.0f;
			star[i].col.x = 1.0f;
			star[i].col.y = 1.0f;
			star[i].col.z = 0.0f;
			star[i].col.w = 1.0f;
		}
		else
		{
			star[i].pos.x = cos(XMConvertToRadians(i) * 36);
			star[i].pos.y = sin(XMConvertToRadians(i) * 36);
			star[i].pos.z = 0.0f;
			star[i].pos.w = 1.0f;
			star[i].col.x = 1.0f;
			star[i].col.y = 0.0f;
			star[i].col.z = 0.0f;
			star[i].col.w = 1.0f;

			if (i % 2 == 0)
			{
				star[i].pos.x = cos(XMConvertToRadians(i) * 36) / 2;
				star[i].pos.y = sin(XMConvertToRadians(i) * 36) / 2;
			}

		}
	}

	for (int i = 0; i < 12; ++i)
	{
		star[i].norm.x = 0;
		star[i].norm.y = 1;
		star[i].norm.z = 0;
	}


	uint indices[60] = { 0, 2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 6, 5, 0, 7, 6, 0, 8, 7, 0, 9, 8, 0, 10, 9, 0, 1, 10, 11, 1, 2, 11, 2, 3, 11, 3, 4, 11, 4, 5, 11, 5, 6, 11, 6, 7, 11, 7, 8, 11, 8, 9, 11, 9, 10, 11, 10, 1 };



	D3D11_BUFFER_DESC vb;
	ZeroMemory(&vb, sizeof(vb));

	vb.Usage = D3D11_USAGE_IMMUTABLE;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = NULL;
	vb.ByteWidth = sizeof(SIMPLE_VERTEX) * 12;
	vb.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA inDat;
	inDat.pSysMem = star;
	inDat.SysMemPitch = 0;
	inDat.SysMemSlicePitch = 0;

	device->CreateBuffer(&vb, &inDat, &vertBuff);

	D3D11_BUFFER_DESC iB;
	ZeroMemory(&iB, sizeof(iB));

	iB.Usage = D3D11_USAGE_IMMUTABLE;
	iB.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iB.ByteWidth = sizeof(unsigned int) * 60;
	iB.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA intDat;
	intDat.pSysMem = indices;
	intDat.SysMemPitch = 0;
	intDat.SysMemSlicePitch = 0;

	device->CreateBuffer(&iB, &intDat, &ibuff);
#pragma endregion

#pragma region Skybox
	D3D11_BUFFER_DESC vbSky;
	ZeroMemory(&vbSky, sizeof(vbSky));

	vbSky.Usage = D3D11_USAGE_IMMUTABLE;
	vbSky.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbSky.CPUAccessFlags = NULL;
	vbSky.ByteWidth = sizeof(_OBJ_VERT_) * 776;
	vbSky.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA inDatSky;
	inDatSky.pSysMem = Cube_data;
	inDatSky.SysMemPitch = 0;
	inDatSky.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbSky, &inDatSky, &vertBuffSky);

	D3D11_BUFFER_DESC ibSky;
	ZeroMemory(&ibSky, sizeof(ibSky));

	ibSky.Usage = D3D11_USAGE_IMMUTABLE;
	ibSky.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibSky.ByteWidth = sizeof(unsigned int) * 1692;
	ibSky.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA intDatSky;
	intDatSky.pSysMem = Cube_indicies;
	intDatSky.SysMemPitch = 0;
	intDatSky.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibSky, &intDatSky, &ibuffSky);
#pragma endregion

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

#pragma region Shaders
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vShade);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pShade);

	device->CreateVertexShader(Star_VS, sizeof(Star_VS), NULL, &vShade2);
	device->CreatePixelShader(Star_PS, sizeof(Star_PS), NULL, &pShade2);

	device->CreateVertexShader(OBJ_VS, sizeof(OBJ_VS), NULL, &vShade3);
	device->CreatePixelShader(OBJ_PS, sizeof(OBJ_PS), NULL, &pShade3);

    device->CreateVertexShader(Box_VS, sizeof(Box_VS), NULL, &vShade4);
	device->CreatePixelShader(Box_PS, sizeof(Box_PS), NULL, &pShade4);

    device->CreateVertexShader(LightTest_VS, sizeof(LightTest_VS), NULL, &vShade5);
	device->CreatePixelShader(LightTest_PS, sizeof(LightTest_PS), NULL, &pShade5);
#pragma endregion

#pragma region Layouts
	D3D11_INPUT_ELEMENT_DESC vLayout[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_COORDINATES", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout, ARRAYSIZE(vLayout), Trivial_VS, sizeof(Trivial_VS), &layout);
	device->CreateInputLayout(vLayout, ARRAYSIZE(vLayout), Box_VS, sizeof(Box_VS), &layout4);

	D3D11_INPUT_ELEMENT_DESC vLayout4[4] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_COORDINATES", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout4, ARRAYSIZE(vLayout4), LightTest_VS, sizeof(LightTest_VS), &layout5);

	D3D11_INPUT_ELEMENT_DESC vLayout2[4] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout2, ARRAYSIZE(vLayout2), Star_VS, sizeof(Star_VS), &layout2);

	D3D11_INPUT_ELEMENT_DESC vLayout3[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_COORDINATES", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout3, ARRAYSIZE(vLayout3), OBJ_VS, sizeof(OBJ_VS), &layout3);
#pragma endregion

#pragma region SamplersAndRasterizers
	D3D11_SAMPLER_DESC descSam = {};
	descSam.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	descSam.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam.MinLOD = -FLT_MAX;
	descSam.MaxLOD = FLT_MAX;
	descSam.MipLODBias = 0.0f;
	descSam.MaxAnisotropy = 1;
	descSam.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&descSam, &samState);

	D3D11_SAMPLER_DESC descSam2 = {};
	descSam2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	descSam2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSam2.MinLOD = -FLT_MAX;
	descSam2.MaxLOD = FLT_MAX;
	descSam2.MipLODBias = 0.0f;
	descSam2.MaxAnisotropy = 1;
	descSam2.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&descSam2, &samState2);

	D3D11_RASTERIZER_DESC descRas = {};
	descRas.FillMode = D3D11_FILL_SOLID;
	descRas.CullMode = D3D11_CULL_FRONT;
	descRas.FrontCounterClockwise = FALSE;
	descRas.DepthBias = 0;
	descRas.SlopeScaledDepthBias = 0.0f;
	descRas.DepthBiasClamp = 0.0f;
	descRas.DepthClipEnable = TRUE;
	descRas.ScissorEnable = FALSE;
	descRas.MultisampleEnable = FALSE;
	descRas.AntialiasedLineEnable = TRUE;

	device->CreateRasterizerState(&descRas, &rasState);

	D3D11_RASTERIZER_DESC descRas2 = {};
	descRas2.FillMode = D3D11_FILL_SOLID;
	descRas2.CullMode = D3D11_CULL_BACK;
	descRas2.FrontCounterClockwise = FALSE;
	descRas2.DepthBias = 0;
	descRas2.SlopeScaledDepthBias = 0.0f;
	descRas2.DepthBiasClamp = 0.0f;
	descRas2.DepthClipEnable = TRUE;
	descRas2.ScissorEnable = FALSE;
	descRas2.MultisampleEnable = FALSE;
	descRas2.AntialiasedLineEnable = TRUE;

	device->CreateRasterizerState(&descRas2, &rasState2);
#pragma endregion

#pragma region Constant Buffer
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));

	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.ByteWidth = sizeof(OBJECT);
	cb.MiscFlags = 0;
	cb.StructureByteStride = sizeof(float);

	device->CreateBuffer(&cb, NULL, &ncBuff);
#pragma endregion

	thread thread1(CreateDDSTextureFromFile, device, L"SunsetSkybox.dds", nullptr, &pSRV, 0);

	thread thread2(CreateDDSTextureFromFile, device, L"stone_0001_c.dds", nullptr, &pSRV2, 0);

	thread thread3(loadOBJ, "cube.obj", verts, uvs, norms, inds);

#pragma region OBJ code
	NEW_VERTEX objCube[8];

	for (int i = 0; i < verts.size(); ++i)
	{
		objCube[i].pos = verts[i];
	}
	for (int i = 0; i < verts.size(); ++i)
	{
		//objCube[i].uvs = uvs[i];
	}
	for (int i = 0; i < norms.size(); ++i)
	{
		objCube[i].norms = norms[i];
	}

	uint inds1[234];

	for (int i = 0; i < inds.size(); ++i)
	{
		inds1[i] = inds[i];
	}


	D3D11_BUFFER_DESC vbCube;
	ZeroMemory(&vbCube, sizeof(vbCube));
	
	vbCube.Usage = D3D11_USAGE_IMMUTABLE;
	vbCube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbCube.CPUAccessFlags = NULL;
	vbCube.ByteWidth = sizeof(NEW_VERTEX) * 8;
	vbCube.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA inDatCube;
	inDatCube.pSysMem = objCube;
	inDatCube.SysMemPitch = 0;
	inDatCube.SysMemSlicePitch = 0;
	
	device->CreateBuffer(&vbCube, &inDatCube, &vertBuffCube);
	
	D3D11_BUFFER_DESC ibCube;
	ZeroMemory(&ibCube, sizeof(ibCube));
	
	ibCube.Usage = D3D11_USAGE_IMMUTABLE;
	ibCube.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibCube.ByteWidth = sizeof(unsigned int) * 234;
	ibCube.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA intDatCube;
	intDatCube.pSysMem = inds1;
	intDatCube.SysMemPitch = 0;
	intDatCube.SysMemSlicePitch = 0;
	
	device->CreateBuffer(&ibCube, &intDatCube, &ibuffCube);
#pragma endregion

#pragma region LightCode
	//Directional Light
	light.dir = { -1.0f, -0.75f, 0.0f };
	light.color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Point Light
	light.color2 = { .0f, 1.0f, 1.0f, 1.0f };
	light.dir2 = { -1.0f, -0.75f, 0.0f };
	light.pos = { 2.5f, -1.5f, 0.0f };
	light.lightRadius = 10.0f;

	//Spotlight
	light.color3 = { 1.0f, 0.0f, 0.0f, 1.0f };
	light.dir3 = { -1.0f, -1.0f, 0.0f };
	light.pos2 = { 2.5f, -1.5f, 0.0f };
	light.coneRatio = 0.93f;

	D3D11_BUFFER_DESC cbLight;
	ZeroMemory(&cbLight, sizeof(cbLight));
	
	cbLight.Usage = D3D11_USAGE_DYNAMIC;
	cbLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbLight.ByteWidth = sizeof(LIGHT);

	D3D11_SUBRESOURCE_DATA intLight;
	intLight.pSysMem = &light;
	intLight.SysMemPitch = 0;
	intLight.SysMemSlicePitch = 0;

	device->CreateBuffer(&cbLight, &intLight, &lightBuffer);

#pragma endregion

#pragma region CubeForLighting
	D3D11_BUFFER_DESC vbLTest;
	ZeroMemory(&vbLTest, sizeof(vbLTest));

	vbLTest.Usage = D3D11_USAGE_IMMUTABLE;
	vbLTest.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbLTest.CPUAccessFlags = NULL;
	vbLTest.ByteWidth = sizeof(_OBJ_VERT_) * 776;
	vbLTest.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA inDatLTest;
	inDatLTest.pSysMem = Cube_data;
	inDatLTest.SysMemPitch = 0;
	inDatLTest.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbLTest, &inDatLTest, &vertBuffLTest);

	D3D11_BUFFER_DESC ibLTest;
	ZeroMemory(&ibLTest, sizeof(ibLTest));

	ibLTest.Usage = D3D11_USAGE_IMMUTABLE;
	ibLTest.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibLTest.ByteWidth = sizeof(unsigned int) * 1692;
	ibLTest.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA intDatLTest;
	intDatLTest.pSysMem = Cube_indicies;
	intDatLTest.SysMemPitch = 0;
	intDatLTest.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibLTest, &intDatLTest, &ibuffLTest);
#pragma endregion

#pragma region Quad verts
	NEW_VERTEX quad[4];

	quad[0].pos.x = -5;
	quad[0].pos.y = -1;
	quad[0].pos.z = 5;
	quad[0].uvs.x = 0;
	quad[0].uvs.y = 0;
	quad[0].uvs.z = 0;
	quad[0].norms.x = 0;
	quad[0].norms.y = 1;
	quad[0].norms.z = 0;

	quad[1].pos.x = 5;
	quad[1].pos.y = -1;
	quad[1].pos.z = 5;
	quad[1].uvs.x = 1;
	quad[1].uvs.y = 0;
	quad[1].uvs.z = 0;
	quad[1].norms.x = 0;
	quad[1].norms.y = 1;
	quad[1].norms.z = 0;

	quad[2].pos.x = -5;
	quad[2].pos.y = -1;
	quad[2].pos.z = -5;
	quad[2].uvs.x = 0;
	quad[2].uvs.y = 1;
	quad[2].uvs.z = 0;
	quad[2].norms.x = 0;
	quad[2].norms.y = 1;
	quad[2].norms.z = 0;
	
	quad[3].pos.x = 5;
	quad[3].pos.y = -1;
	quad[3].pos.z = -5;
	quad[3].uvs.x = 1;
	quad[3].uvs.y = 1;
	quad[3].uvs.z = 0;
	quad[3].norms.x = 0;
	quad[3].norms.y = 1;
	quad[3].norms.z = 0;

	uint indic[6] = { 0, 1, 2, 1, 3, 2,};

	D3D11_BUFFER_DESC vbQ;
	ZeroMemory(&vbQ, sizeof(vbQ));

	vbQ.Usage = D3D11_USAGE_IMMUTABLE;
	vbQ.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbQ.CPUAccessFlags = NULL;
	vbQ.ByteWidth = sizeof(NEW_VERTEX) * 4;
	vbQ.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA inDatQ;
	inDatQ.pSysMem = quad;
	inDatQ.SysMemPitch = 0;
	inDatQ.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbQ, &inDatQ, &vertBuffQuad);

	D3D11_BUFFER_DESC iBQ;
	ZeroMemory(&iBQ, sizeof(iBQ));

	iBQ.Usage = D3D11_USAGE_IMMUTABLE;
	iBQ.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBQ.ByteWidth = sizeof(unsigned int) * 6;
	iBQ.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA intDatQ;
	intDatQ.pSysMem = indic;
	intDatQ.SysMemPitch = 0;
	intDatQ.SysMemSlicePitch = 0;

	device->CreateBuffer(&iBQ, &intDatQ, &ibuffQuad);

#pragma endregion

#pragma region Transparency
	D3D11_BLEND_DESC blends;
	ZeroMemory(&blends, sizeof(blends));

	D3D11_RENDER_TARGET_BLEND_DESC renTar;
	ZeroMemory(&renTar, sizeof(renTar));

	renTar.BlendEnable = true;
	renTar.SrcBlend = D3D11_BLEND_SRC_COLOR;
	renTar.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	renTar.BlendOp = D3D11_BLEND_OP_ADD;
	renTar.SrcBlendAlpha = D3D11_BLEND_ONE;
	renTar.DestBlendAlpha = D3D11_BLEND_ZERO;
	renTar.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renTar.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blends.AlphaToCoverageEnable = false;
	blends.RenderTarget[0] = renTar;

	device->CreateBlendState(&blends, &Trans);

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	device->CreateRasterizerState(&cmdesc, &CCWcull);

	cmdesc.FrontCounterClockwise = false;
	device->CreateRasterizerState(&cmdesc, &CWcull);
#pragma endregion
	
#pragma region Matricies
	toObject.worldMatrix = XMMatrixIdentity();
	camera = XMMatrixIdentity();
	toObject2.worldMatrix = XMMatrixIdentity(); 

	camera = XMMatrixIdentity();
	toObject.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.0f), ((float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT) / 2.0f, 0.1f, 100.0f);
	toObject2.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.0f), ((float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT) / 2.0f, 0.1f, 100.0f);
	timer.Signal();
	
	camera2 = XMMatrixTranslation(0,0,-5);

	starMatrix = XMMatrixIdentity();
	starMatrix = XMMatrixTranslation(0, 0, 5);

	boxMatrix = XMMatrixIdentity();
	boxMatrix = XMMatrixTranslation(-5, 0.5f, 5);
	boxMatrix2 = XMMatrixIdentity();
	boxMatrix2 = XMMatrixTranslation(-7, 0.5f, 7);
	boxMatrix3 = XMMatrixIdentity();
	boxMatrix3 = XMMatrixTranslation(-3, 0.5f, 3);

	objMatrix = XMMatrixIdentity();
	objMatrix = XMMatrixTranslation(5, 0, 5);

	quadMatrix = XMMatrixIdentity();
	quadMatrix = XMMatrixTranslation(0, 0, 5);
#pragma endregion

	thread1.join();
	thread2.join();
	thread3.join();
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
#pragma region CameraControl
	if (GetAsyncKeyState('W'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, 0.005f), camera);
		camera2 = XMMatrixMultiply(XMMatrixTranslation(0, 0, 0.005f), camera2);
	}
	if (GetAsyncKeyState('A'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(-0.005f, 0, 0), camera);
		camera2 = XMMatrixMultiply(XMMatrixTranslation(-0.005f, 0, 0), camera2);
	}
	if (GetAsyncKeyState('S'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, -0.005f), camera);
		camera2 = XMMatrixMultiply(XMMatrixTranslation(0, 0, -0.005f), camera2);
	}
	if (GetAsyncKeyState('D'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0.005f, 0, 0), camera);
		camera2 = XMMatrixMultiply(XMMatrixTranslation(0.005f, 0, 0), camera2);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0.001f, 0));
		camera2 = XMMatrixMultiply(camera2, XMMatrixTranslation(0, 0.001f, 0));
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, -0.001f, 0));
		camera2 = XMMatrixMultiply(camera2, XMMatrixTranslation(0, -0.001f, 0));
	}

	XMVECTOR pos = camera.r[3];
	camera.r[3] = g_XMIdentityR3;
	XMVECTOR pos2 = camera2.r[3];
	camera2.r[3] = g_XMIdentityR3;

	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(-0.001f));
		camera2 = XMMatrixMultiply(camera2, XMMatrixRotationY(-0.001f));
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(0.001f));
		camera2 = XMMatrixMultiply(camera2, XMMatrixRotationY(0.001f));
	}
	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(-0.001f), camera);
		camera2 = XMMatrixMultiply(XMMatrixRotationX(-0.001f), camera2);
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(0.001f), camera);
		camera2 = XMMatrixMultiply(XMMatrixRotationX(0.001f), camera2);
	}

	camera.r[3] = pos;
	camera2.r[3] = pos2;

	light.pos2 = { camera.r[3].m128_f32[0], camera.r[3].m128_f32[1], camera.r[3].m128_f32[2] };
	light.dir3 = { camera.r[2].m128_f32[0], camera.r[2].m128_f32[1], camera.r[2].m128_f32[2] };

	light.camPos = { camera.r[3].m128_f32[0], camera.r[3].m128_f32[1], camera.r[3].m128_f32[2], camera.r[3].m128_f32[3] };

#pragma endregion 

#pragma region Directional Light Control
	if (GetAsyncKeyState('Y'))
	{
		light.dir = { light.dir.x, light.dir.y + (float)(timer.Delta())/100, light.dir.z };
	}
	if (GetAsyncKeyState('H'))
	{
		light.dir = { light.dir.x, light.dir.y - (float)(timer.Delta())/100, light.dir.z };
	}
#pragma endregion

#pragma region Point Light Control
	if (GetAsyncKeyState('J'))
	{
		light.pos = { light.pos.x - (float)(timer.Delta()) / 100, light.pos.y, light.pos.z };
	}
	if (GetAsyncKeyState('L'))
	{
		light.pos = { light.pos.x + (float)(timer.Delta()) / 100, light.pos.y, light.pos.z };
	}
	if (GetAsyncKeyState('I'))
	{
		light.pos = { light.pos.x, light.pos.y, light.pos.z + (float)(timer.Delta()) / 100 };
	}
	if (GetAsyncKeyState('K'))
	{
		light.pos = { light.pos.x, light.pos.y, light.pos.z - (float)(timer.Delta()) / 100 };
	}
#pragma endregion

	context->OMSetRenderTargets(1, &pView, pDSV);
	context->OMSetBlendState(0, 0, 0xffffffff);
	context->PSSetConstantBuffers(0, 1, &lightBuffer);

	D3D11_MAPPED_SUBRESOURCE subLight;
	context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &subLight);
	memcpy(subLight.pData, &light, sizeof(light));
	context->Unmap(lightBuffer, 0);

#pragma region ViewPort1

	context->RSSetViewports(1, &viewport);

	float colors[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(pView, colors);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
	/////////////////////////////////////////////////////////////////////////
	// skybox
	toObject.viewMatrix = XMMatrixInverse(nullptr, camera);
	toObject.worldMatrix = XMMatrixTranslation(camera.r[3].m128_f32[0], camera.r[3].m128_f32[1], camera.r[3].m128_f32[2]);

	D3D11_MAPPED_SUBRESOURCE sub;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT offsetSky = 0;
	UINT strideSky = sizeof(_OBJ_VERT_);

	context->IASetVertexBuffers(0, 1, &vertBuffSky, &strideSky, &offsetSky);
	context->IASetIndexBuffer(ibuffSky, DXGI_FORMAT_R32_UINT, offsetSky);

	context->VSSetShader(vShade, NULL, 0);
	context->PSSetShader(pShade, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout);

	context->RSSetState(rasState);

	context->DrawIndexed(1692, 0, 0);

	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// star
	starMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), starMatrix);
	toObject.worldMatrix = starMatrix;

	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	UINT offset = 0;
	UINT stride = sizeof(SIMPLE_VERTEX);
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->IASetIndexBuffer(ibuff, DXGI_FORMAT_R32_UINT, offset);
	context->VSSetShader(vShade2, NULL, 0);
	context->PSSetShader(pShade2, NULL, 0);
	context->IASetInputLayout(layout2);
	context->RSSetState(rasState2);
	context->DrawIndexed(60, 0, 0);

	
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	//Quad
	//starMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), starMatrix);
	toObject.worldMatrix = quadMatrix;

	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	UINT offsetQuad = 0;
	UINT strideQuad = sizeof(NEW_VERTEX);
	context->IASetVertexBuffers(0, 1, &vertBuffQuad, &strideQuad, &offsetQuad);
	context->IASetIndexBuffer(ibuffQuad, DXGI_FORMAT_R32_UINT, offsetQuad);
	context->VSSetShader(vShade5, NULL, 0);
	context->PSSetShader(pShade5, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->IASetInputLayout(layout5);
	//context->PSSetSamplers(0, 1, &samState2);
	context->RSSetState(rasState2);
	context->DrawIndexed(6, 0, 0);
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// OBJ File
	//objMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians((float)timer.Delta() * 0.5f)), objMatrix);
	//toObject.worldMatrix = objMatrix;
	//
	//context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	//memcpy(sub.pData, &toObject, sizeof(toObject));
	//context->Unmap(ncBuff, 0);
	//
	//UINT offsetOBJ = 0;
	//UINT strideOBJ = sizeof(XMFLOAT3);
	//context->IASetVertexBuffers(0, 1, &vertBuffCube, &strideOBJ, &offsetOBJ);
	//context->IASetIndexBuffer(ibuffCube, DXGI_FORMAT_R32_UINT, offsetOBJ);
	//context->VSSetShader(vShade3, NULL, 0);
	//context->PSSetShader(pShade3, NULL, 0);
	//context->PSSetShaderResources(0, 1, &pSRV2);
	//context->IASetInputLayout(layout3);
	//context->RSSetState(rasState2);
	//context->DrawIndexed(234, 0, 0);

	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// Box
	//boxMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), boxMatrix);
	toObject.worldMatrix = boxMatrix;

	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	context->OMSetBlendState(Trans, blendFactor, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE sub3;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub3);
	memcpy(sub3.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT offsetLTest = 0;
	UINT strideLTest = sizeof(_OBJ_VERT_);

	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	////Box2
	toObject.worldMatrix = boxMatrix2;

	D3D11_MAPPED_SUBRESOURCE sub3z;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub3z);
	memcpy(sub3z.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	////Box2
	toObject.worldMatrix = boxMatrix3;

	D3D11_MAPPED_SUBRESOURCE sub3x;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub3x);
	memcpy(sub3x.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	/////////////////////////////////////////////////////////////////////////
#pragma endregion
	
#pragma region ViewPort2
	context->RSSetViewports(1, &viewport2);
	context->OMSetBlendState(0, 0, 0xffffffff);
	light.camPos = { camera2.r[3].m128_f32[0], camera2.r[3].m128_f32[1], camera2.r[3].m128_f32[2], camera2.r[3].m128_f32[3] };

	/////////////////////////////////////////////////////////////////////////
	// skybox

	toObject2.viewMatrix = XMMatrixInverse(nullptr, camera2);
	toObject2.worldMatrix = XMMatrixTranslation(camera2.r[3].m128_f32[0], camera2.r[3].m128_f32[1], camera2.r[3].m128_f32[2]);
	toObject2.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), ((float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT) / 2.0f, 0.1f, 100.0f);

	D3D11_MAPPED_SUBRESOURCE suba;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &suba);
	memcpy(suba.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetVertexBuffers(0, 1, &vertBuffSky, &strideSky, &offsetSky);
	context->IASetIndexBuffer(ibuffSky, DXGI_FORMAT_R32_UINT, offsetSky);

	context->VSSetShader(vShade, NULL, 0);
	context->PSSetShader(pShade, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout);

	context->RSSetState(rasState);

	context->DrawIndexed(1692, 0, 0);

	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// Light
	//constbuffPerFrame.light = light;
	//
	//context->VSSetConstantBuffers(0, 1, &lightBuffer);
	//
	//context->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	//
	//context->PSSetConstantBuffers(0, 1, &lightBuffer);
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// star
	starMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), starMatrix);
	toObject2.worldMatrix = starMatrix;

	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	//UINT offset = 0;
	//UINT stride = sizeof(SIMPLE_VERTEX);
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->IASetIndexBuffer(ibuff, DXGI_FORMAT_R32_UINT, offset);
	context->VSSetShader(vShade2, NULL, 0);
	context->PSSetShader(pShade2, NULL, 0);
	context->IASetInputLayout(layout2);
	context->RSSetState(rasState2);
	context->DrawIndexed(60, 0, 0);


	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	//Quad
	//starMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), starMatrix);
	toObject2.worldMatrix = quadMatrix;

	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	//UINT offsetQuad = 0;
	//UINT strideQuad = sizeof(NEW_VERTEX);
	context->IASetVertexBuffers(0, 1, &vertBuffQuad, &strideQuad, &offsetQuad);
	context->IASetIndexBuffer(ibuffQuad, DXGI_FORMAT_R32_UINT, offsetQuad);
	context->VSSetShader(vShade5, NULL, 0);
	context->PSSetShader(pShade5, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->IASetInputLayout(layout5);
	//context->PSSetSamplers(0, 1, &samState2);
	context->RSSetState(rasState2);
	context->DrawIndexed(6, 0, 0);
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// OBJ File
	//objMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians((float)timer.Delta() * 0.5f)), objMatrix);
	//toObject2.worldMatrix = objMatrix;
	//
	//context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	//memcpy(sub.pData, &toObject2, sizeof(toObject2));
	//context->Unmap(ncBuff, 0);
	//
	////UINT offsetOBJ = 0;
	////UINT strideOBJ = sizeof(XMFLOAT3);
	//context->IASetVertexBuffers(0, 1, &vertBuffCube, &strideOBJ, &offsetOBJ);
	//context->IASetIndexBuffer(ibuffCube, DXGI_FORMAT_R32_UINT, offsetOBJ);
	//context->VSSetShader(vShade3, NULL, 0);
	//context->PSSetShader(pShade3, NULL, 0);
	//context->PSSetShaderResources(0, 1, &pSRV2);
	//context->IASetInputLayout(layout3);
	//context->RSSetState(rasState2);
	//context->DrawIndexed(234, 0, 0);

	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// Box
	//boxMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), boxMatrix);
	toObject2.worldMatrix = boxMatrix;
	context->OMSetBlendState(Trans, blendFactor, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE subc;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &subc);
	memcpy(subc.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	///Box2
	toObject2.worldMatrix = boxMatrix2;
	context->OMSetBlendState(Trans, blendFactor, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE subcz;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &subcz);
	memcpy(subcz.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	///Box3
	toObject2.worldMatrix = boxMatrix3;
	context->OMSetBlendState(Trans, blendFactor, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE subca;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &subca);
	memcpy(subca.pData, &toObject2, sizeof(toObject2));
	context->Unmap(ncBuff, 0);

	context->VSSetConstantBuffers(0, 1, &ncBuff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	context->IASetVertexBuffers(0, 1, &vertBuffLTest, &strideLTest, &offsetLTest);
	context->IASetIndexBuffer(ibuffLTest, DXGI_FORMAT_R32_UINT, offsetLTest);

	context->VSSetShader(vShade4, NULL, 0);
	context->PSSetShader(pShade4, NULL, 0);
	context->PSSetShaderResources(0, 1, &pSRV2);
	context->PSSetSamplers(0, 1, &samState);

	context->IASetInputLayout(layout4);

	context->RSSetState(rasState2);

	context->RSSetState(CCWcull);
	context->DrawIndexed(1692, 0, 0);

	context->RSSetState(CWcull);
	context->DrawIndexed(1692, 0, 0);

	/////////////////////////////////////////////////////////////////////////
#pragma endregion
	
	swapChain->Present(0, 0);

	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{

	swapChain->Release();
	pView->Release();
	device->Release();
	context->Release();
	vertBuff->Release();
	ibuff->Release();
	vertBuffSky->Release();
	ibuffSky->Release();
	vertBuffCube->Release();
	ibuffCube->Release();
	layout->Release();
	vShade->Release();
	pShade->Release();
	ncBuff->Release();
	pDSV->Release();
	pDepthStencil->Release();
	rasState->Release();
	pSRV->Release();
	pSRV2->Release();
	samState->Release();
	samState2->Release();
	vShade2->Release();
	pShade2->Release();
	layout2->Release();
	layout3->Release();
	layout4->Release();
	layout5->Release();
	rasState2->Release();
	vShade3->Release();
	pShade3->Release();
	lightBuffer->Release();
	vertBuffLTest->Release();
	ibuffLTest->Release();
	vShade4->Release();
	pShade4->Release();
	vertBuffQuad->Release();
	ibuffQuad->Release();
	vShade5->Release();
	pShade5->Release();
	Trans->Release();
	CCWcull->Release();
	CWcull->Release();


	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//