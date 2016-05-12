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



#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

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
	UINT numVerts = 0;
	ID3D11InputLayout *layout;
	ID3D11InputLayout *layout2;
	ID3D11InputLayout *layout3;
	ID3D11ShaderResourceView *pSRV = {};
	ID3D11SamplerState* samState;

	ID3D11RasterizerState* rasState;
	ID3D11RasterizerState* rasState2;
	
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vShade;
	ID3D11VertexShader *vShade2;
	ID3D11VertexShader *vShade3;
	ID3D11PixelShader *pShade;
	ID3D11PixelShader *pShade2;
	ID3D11PixelShader *pShade3;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *ncBuff;
	XTime timer;

	XMMATRIX camera;
	XMMATRIX objMatrix;
	XMMATRIX starMatrix;

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	struct SIMPLE_VERTEX
	{
		//float x, y, z, w;
		XMFLOAT4 pos;
		XMFLOAT4 col;
		//float r, g, b, a;
	};

	struct NEW_VERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT2 uvs;
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

	OBJECT toObject;

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
			out_verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			//temp_uvs.push_back(uv);
			out_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			//temp_normals.push_back(normal);
			out_norms.push_back(normal);
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

			for (unsigned int i = 0; i < vertexIndices.size(); i++)
			{
				uint vertIndex = vertexIndices[i];
				//XMFLOAT3 vert = temp_vertices[vertIndex - 1];
				out_inds.push_back(vertIndex);
			}
			//for (unsigned int i = 0; i < uvIndices.size(); i++)
			//{
			//	unsigned int uvIndex = vertexIndices[i];
			//	XMFLOAT2 uv = temp_uvs[uvIndex - 1];
			//	out_uvs.push_back(uv);
			//}
			//for (unsigned int i = 0; i < normalIndices.size(); i++)
			//{
			//	unsigned int normIndex = vertexIndices[i];
			//	XMFLOAT3 norm = temp_normals[normIndex - 1];
			//	out_norms.push_back(norm);
			//}

		}
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
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab 1a Line Land",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
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
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	SIMPLE_VERTEX star[12];
#pragma region Star verts

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

			if (i%2 == 0)
			{
				star[i].pos.x = cos(XMConvertToRadians(i) * 36)/2;
				star[i].pos.y = sin(XMConvertToRadians(i) * 36)/2;
			}
			
		}
	}

#pragma endregion

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
		
	
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vShade);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pShade);

    device->CreateVertexShader(Star_VS, sizeof(Star_VS), NULL, &vShade2);
	device->CreatePixelShader(Star_PS, sizeof(Star_PS), NULL, &pShade2);

	device->CreateVertexShader(OBJ_VS, sizeof(OBJ_VS), NULL, &vShade3);
	device->CreatePixelShader(OBJ_PS, sizeof(OBJ_PS), NULL, &pShade3);
	
	D3D11_INPUT_ELEMENT_DESC vLayout[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_COORDINATES", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{"INSTPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA, 0},
		//{"x_axis", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1,12,D3D11_INPUT_PER_INSTANCE_DATA, 0},
		//{"y_axis", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1,24,D3D11_INPUT_PER_INSTANCE_DATA, 0},
		//{"z_axis", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,1,36,D3D11_INPUT_PER_INSTANCE_DATA, 0}
	};

	device->CreateInputLayout(vLayout, ARRAYSIZE(vLayout), Trivial_VS, sizeof(Trivial_VS), &layout);

	D3D11_INPUT_ELEMENT_DESC vLayout2[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout2, ARRAYSIZE(vLayout2), Star_VS, sizeof(Star_VS), &layout2);

	D3D11_INPUT_ELEMENT_DESC vLayout3[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE_COORDINATES", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(vLayout3, ARRAYSIZE(vLayout3), OBJ_VS, sizeof(OBJ_VS), &layout3);


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
	descRas.FillMode = D3D11_FILL_SOLID;
	descRas.CullMode = D3D11_CULL_BACK;
	descRas.FrontCounterClockwise = FALSE;
	descRas.DepthBias = 0;
	descRas.SlopeScaledDepthBias = 0.0f;
	descRas.DepthBiasClamp = 0.0f;
	descRas.DepthClipEnable = TRUE;
	descRas.ScissorEnable = FALSE;
	descRas.MultisampleEnable = FALSE;
	descRas.AntialiasedLineEnable = TRUE;

	device->CreateRasterizerState(&descRas, &rasState2);


	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));

	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.ByteWidth = sizeof(OBJECT);
	cb.MiscFlags = 0;
	cb.StructureByteStride = sizeof(float);

	device->CreateBuffer(&cb, NULL, &ncBuff);

	CreateDDSTextureFromFile(device, L"SunsetSkybox.dds", nullptr, &pSRV);

	bool hr = loadOBJ("cube.obj", verts, uvs, norms, inds);
	
	NEW_VERTEX objCube[8];

#pragma region OBJ verts

	for (int i = 0; i < verts.size(); ++i)
	{
		objCube[i].pos = verts[i];
	}
	for (int i = 0; i < verts.size(); ++i)
	{
		objCube[i].uvs = uvs[i];
	}
	for (int i = 0; i < norms.size(); ++i)
	{
		objCube[i].norms = norms[i];
	}

#pragma endregion

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

	
	toObject.worldMatrix = XMMatrixIdentity();
	camera = XMMatrixIdentity();

	camera = XMMatrixIdentity();
	toObject.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.0f), (float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT, 0.1f, 100.0f);
	timer.Signal();


	starMatrix = XMMatrixIdentity();
	starMatrix = XMMatrixTranslation(0, 0, 5);

	objMatrix = XMMatrixIdentity();
	objMatrix = XMMatrixTranslation(5, 0, 5);
}



//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
#pragma region CameraControl
	if (GetAsyncKeyState('W'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, 0.001f), camera);
	}
	if (GetAsyncKeyState('A'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(-0.001f, 0, 0), camera);
	}
	if (GetAsyncKeyState('S'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, -0.001f), camera);
	}
	if (GetAsyncKeyState('D'))
	{
		camera = XMMatrixMultiply(XMMatrixTranslation(0.001f, 0, 0), camera);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0.001f, 0));
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, -0.001f, 0));
	}
	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		camera = XMMatrixMultiply(XMMatrixRotationY(-0.001f), camera);
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		camera = XMMatrixMultiply(XMMatrixRotationY(0.001f), camera);
	}
	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(-0.001f), camera);
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		camera = XMMatrixMultiply(XMMatrixRotationX(0.001f), camera);
	}
#pragma endregion 

	context->OMSetRenderTargets(1, &pView, pDSV);

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
	// OBJ File
	objMatrix = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(timer.Delta() * 0.5f)), objMatrix);
	toObject.worldMatrix = objMatrix;

	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	UINT offsetOBJ = 0;
	UINT strideOBJ = sizeof(XMFLOAT3);
	context->IASetVertexBuffers(0, 1, &vertBuffCube, &strideOBJ, &offsetOBJ);
	context->IASetIndexBuffer(ibuffCube, DXGI_FORMAT_R32_UINT, offsetOBJ);
	context->VSSetShader(vShade3, NULL, 0);
	context->PSSetShader(pShade3, NULL, 0);
	context->IASetInputLayout(layout3);
	context->RSSetState(rasState2);
	context->DrawIndexed(234, 0, 0);


	/////////////////////////////////////////////////////////////////////////



	swapChain->Present(0, 0);
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
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
	samState->Release();
	vShade2->Release();
	pShade2->Release();
	layout2->Release();
	rasState2->Release();
	vShade3->Release();
	pShade3->Release();


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