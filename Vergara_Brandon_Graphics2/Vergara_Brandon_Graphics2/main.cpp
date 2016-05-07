//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <string.h>
#include "defines.h"
#include "DDSTextureLoader.h"

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
	ID3D11Buffer *ibuff;
	ID3D11Buffer *ibuffSky;
	UINT numVerts = 0;
	ID3D11InputLayout *layout;
	ID3D11InputLayout *layout2;
	ID3D11ShaderResourceView *pSRV = {};
	ID3D11SamplerState* samState;

	ID3D11RasterizerState* rasState;
	ID3D11RasterizerState* rasState2;
	
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vShade;
	ID3D11VertexShader *vShade2;
	ID3D11PixelShader *pShade;
	ID3D11PixelShader *pShade2;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *ncBuff;
	XTime timer;

	XMMATRIX camera;

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

	// BEGIN PART 4
	// TODO: PART 4 STEP 1
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
	
	// TODO: PART 5 STEP 2b
	
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

	
	toObject.worldMatrix = XMMatrixIdentity();
	camera = XMMatrixIdentity();

	camera = XMMatrixIdentity();
	toObject.projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.0f), (float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT, 0.1f, 100.0f);
	timer.Signal();


	starMatrix = XMMatrixIdentity();
	starMatrix = XMMatrixTranslation(0, 0, 5);
}



//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
#pragma region CameraControl

	// replace with "Camera" matrix
	if (GetAsyncKeyState('W'))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, 0.001f), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState('A'))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixTranslation(-0.001f, 0, 0), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState('S'))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixTranslation(0, 0, -0.001f), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState('D'))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixTranslation(0.001f, 0, 0), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0.001f, 0));
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, -0.001f, 0));
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(0.001f));
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(camera, XMMatrixRotationY(-0.001f));
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixRotationX(0.001f), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}

	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		//XMMATRIX viewTemp = camera;
		//camera = XMMatrixIdentity();
		camera = XMMatrixMultiply(XMMatrixRotationX(-0.001f), camera);
		//camera = XMMatrixMultiply(camera, viewTemp);
	}
#pragma endregion 

	//XMMATRIX rotation = XMMatrixRotationY((float)timer.Delta() * 0.005f);
	//toObject.worldMatrix = XMMatrixMultiply(rotation, toObject.worldMatrix);


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